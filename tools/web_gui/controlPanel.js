const PACKETS_INCOME = {
	'setfen': 			['42751315', (m)=>{ console.log('setfen: ', m); }],
};
const PACKETS_OUTCOME = {
	'setfen': 			['93379838', (m)=>{ console.log('setfen: ', m); }],
	'setboard': 		['70988515', (m)=>{ console.log('setboard: ', m); }],
	'updboard': 		['98716453', (m)=>{ console.log('updboard: ', m); }],
};

const BAUD_RATE = 115200;
const SERIAL_READ_WAIT_TIMEOUT = 20; // ms
const PACKETTYPE__STATE_UPDATE = 0b10110001;

///////////////////////////////////////////////////////////////////////////

const BOARD_ROTATE180 = true;
const BOARD_FLIPX = true;

const SQUARE_MARGIN = 1;

const SQUARE_COLOR_INACTIVE = 'rgb(231, 231, 231)';
const SQUARE_COLOR_HOVER = 'rgb(196, 255, 191)';
const SQUARE_COLOR_CLICKED = 'rgb(255, 234, 118)';


////////////////////////////////////////////////////////////////////////////
////////////////////////////////// SERIAL //////////////////////////////////
////////////////////////////////////////////////////////////////////////////

if ("serial" in navigator === false)
	alert("SerialPort isn't supported!!!");

function serialReceived(msg) {
	console.log(msg);
	for (const packetsKey in PACKETS_INCOME) {
		const packetEntry = PACKETS_INCOME[packetsKey];
		const packetId = packetEntry[0];
		if (msg.startsWith(packetId)) {
			packetEntry[1](msg.substring(packetId.length));
			break;
		}
	}
}

var port = null;

async function readSerial() {
	const reader = port.readable.getReader();
	let chunks = [];

	function decodeChunks(chunks) {
		const length = chunks.reduce((accum, curval) => accum + curval.length, 0);
		// console.log('length:', length);
		let mergedArray = new Uint8Array(length);
		let offset = 0;
		chunks.forEach(item => {
			mergedArray.set(item, offset);
			offset += item.length;
		});
		return (new TextDecoder()).decode(mergedArray);
	}

	try {
		let timeoutId = 0;
		function readingComplete() {
			serialReceived(decodeChunks(chunks))
			chunks = []; // not thread safe, but would work for now
		}
		while (true) {
			const { value, done } = await reader.read();
			if (done) {
				console.log("Stream complete");
				readingComplete();
				break;
			}
			if (value) {
				// console.log('chunk:', value);
				chunks.push(value);

				clearTimeout(timeoutId);
				timeoutId = setTimeout(readingComplete, SERIAL_READ_WAIT_TIMEOUT);
			}
		}
	} catch (error) {
		console.error(error);
	} finally {
		reader.releaseLock();
	}
}

async function openPort() {
	await port.open({baudRate: BAUD_RATE});
	console.log('Serial port opened: ', port.getInfo());
	

	while (port.readable) {
		await readSerial();
	}
	console.log('after while(port.readable);');
}

async function sendStringToSerial(msg) {
	const writer = port.writable.getWriter();
	const encoder = new TextEncoder();
	const encoded = encoder.encode(msg);
	await writer.write(encoded);
	writer.releaseLock();
	console.log('%c' + msg, 'color: #bada55');
}

async function sendSerialPacket(packetId, msg) {
	const zeroPad = (number, places) => String(number).padStart(places, '0');
	await sendStringToSerial(packetId + zeroPad(msg.length, 4) + msg);
}

//--//--// Port connection
document.querySelector('#btnConnect').addEventListener('click', async () => {
	navigator.serial.getPorts().then(async (ports) => {
		console.log(ports);
		if (ports.length == 1) {
			port = ports[0];
			await openPort();
		}
	}).then(async () => {
		// const filters = [{usbVendorId: 0x1a86, usbProductId: 0x7523}]; // arduino
		const filters = [{usbVendorId: 4292}]; // esp, all???
		// const filters = [];
		port = await navigator.serial.requestPort({filters});
		if (!port)
			return consolge.log("couldn't request ports!");
		await openPort();
	});

});
document.querySelector('#btnDisconnect').addEventListener('click', async () => {
	if (!port)
		return console.log('no open port found!');
	await port.close();
	console.log('Disconnected');
});

document.getElementById('btnSend').addEventListener('click', async function(evt) {
	await sendStringToSerial(document.getElementById('txbxMsg').value);
});

document.getElementById('btnSendInitialization').addEventListener('click', async function(evt) {
	await sendBoardFen();
	await sendSetBoardState();
});



async function sendBoardFen() {
	await sendSerialPacket(PACKETS_OUTCOME['setfen'][0], board.fen());
}
function getBoardStateMsg() {
	let state = '';
	for (let idx = 0; idx < 64; ++idx)
		state += squares[idx].state ? '1' : '0';
	return state;
}
async function sendBoardUpdate() {
	await sendSerialPacket(PACKETS_OUTCOME['updboard'][0], getBoardStateMsg());
}
async function sendSetBoardState() {
	await sendSerialPacket(PACKETS_OUTCOME['setboard'][0], getBoardStateMsg());
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////// CHESS ///////////////////////////////////
////////////////////////////////////////////////////////////////////////////

function setSquaresToBoard() {
	let occupyMap = {};
	for (const key in board.position()) {
		const col = key.charCodeAt(0) - 'a'.charCodeAt(0);
		const row = key.charCodeAt(1) - '1'.charCodeAt(0);
		const idx = parseInt(row) * 8 + parseInt(col);
		// console.log(key, '=>', idx);
		occupyMap[idx] = 1;
	}
	for (let i = 0; i < squares.length; ++i) {
		squares[i].stateSet(i in occupyMap ? 1 : 0);
	}
}

// document.getElementById('txbxFen').value = INITIAL_FEN;
document.getElementById('btnSetFen').addEventListener('click', async function(evt) {
	board.position(document.getElementById('txbxFen').value);
});
document.getElementById('btnSetBoard').addEventListener('click', async function(evt) {
	setSquaresToBoard();
});
function updateTxbxFenValue(v) {
	document.getElementById('txbxFen').value = v;
}
updateTxbxFenValue(document.getElementById('slFens').value);
document.getElementById('slFens').addEventListener('change', async function(evt) {
	updateTxbxFenValue(evt.target.value);
});
document.getElementById('slFens').addEventListener('wheel', function(evt) {
    if (evt.deltaY < 0)
		this.selectedIndex = Math.max(this.selectedIndex - 1, 0);
	else
		this.selectedIndex = Math.min(this.selectedIndex + 1, this.length - 1);
	updateTxbxFenValue(evt.target.value);
	evt.preventDefault();
});
document.getElementById('btnGetFen').addEventListener('click', async function(evt) {
	document.getElementById('txbxFen').value = board.fen();
});


// https://konvajs.org/docs/events/Keyboard_Events.html

let squares = [];

let stage = new Konva.Stage({
	container: 'controlPanel',
	width: BOARDSIZE,
	height: BOARDSIZE,
	rotation: BOARD_ROTATE180 ? 180 : 0,
	x: BOARD_ROTATE180 != BOARD_FLIPX ? BOARDSIZE : 0,
	y: BOARD_ROTATE180 ? BOARDSIZE : 0,
	scaleX: BOARD_FLIPX ? -1 : 1
  });
  squareWidth = (stage.width() - 10) / 8;
  squareHeight = (stage.height() - 10) / 8;

let layer = new Konva.Layer();

for (let i = 0; i < 64; ++i) {
	let rect = new Konva.Rect({
	  x: (i % 8) * (squareWidth + SQUARE_MARGIN) + SQUARE_MARGIN, y: Math.floor(i / 8) * (squareHeight + SQUARE_MARGIN) + SQUARE_MARGIN,
	  width: squareWidth, height: squareHeight,
	  name: i
	});

	rect.stateToggle = function() {
		this.stateSet(!this.state);
	};
	rect.stateSet = function(val) {
		this.state = val;
		this.updateFill();
	}
	rect.updateFill = function() {
		this.fill(this.state ? SQUARE_COLOR_CLICKED : SQUARE_COLOR_INACTIVE);
	}

	// rect.state = i < 16 || i > 47;
	// rect.state = i in occupyMap ? 1 : 0;
	rect.updateFill();


	rect.on('mouseenter', function(evt) {
		// console.log(evt.evt);
        this.stroke('black');
        this.strokeWidth(2);
		if (evt.evt.buttons === 1) {
			if (evt.evt.ctrlKey)
				this.stateSet(1);
			else if (evt.evt.shiftKey)
				this.stateSet(0);
			else
				this.stateToggle();
			sendBoardUpdate();
		} else if (evt.evt.buttons === 2) {
			this.stateSet(0);
			sendBoardUpdate();
		}
		// console.log(evt.target.name());
	});
	rect.on('mouseleave', function() {
        this.strokeWidth(0);
	});
	rect.on('click', function(evt) {
		// console.log(evt);
		this.stateToggle();
		sendBoardUpdate();
	});

	squares.push(rect);
	layer.add(rect);
}
setSquaresToBoard();

stage.add(layer);


var container = stage.container();
container.tabIndex = 1;
container.focus();
container.addEventListener('keydown', function (e) {
  if (e.code === 'KeyC') { // space
	for (let i = 0; i < squares.length; ++i) {
		squares[i].stateSet(0);
	}
  } else {
	return;
  }
  e.preventDefault();
});