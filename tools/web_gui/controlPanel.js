const BAUD_RATE = 115200

const PACKETTYPE__STATE_UPDATE = 0b10110001;

///////////////////////////////////////////////////////////////////////////

const SQUARE_COLOR_INACTIVE = 'rgb(231, 231, 231)';
const SQUARE_COLOR_HOVER = 'rgb(196, 255, 191)';
const SQUARE_COLOR_CLICKED = 'rgb(255, 234, 118)';

const SQUARE_MARGIN = 1;


////////////////////////////////////////////////////////////////////////////

if ("serial" in navigator === false)
	alert("SerialPort isn't supported!!!");

var port = null;
var writer = null;
document.querySelector('#btnConnect').addEventListener('click', async () => {
	// const filters = [{usbVendorId: 0x1a86, usbProductId: 0x7523}];
	const filters = [];
	const port = await navigator.serial.requestPort({filters});
	if (!port)
		return;
	await port.open({baudRate: BAUD_RATE});
	console.log('opened');

	writer = port.writable.getWriter();

	while (port.readable) {
		const reader = port.readable.getReader();
		try {
			while (true) {
				const { value, done } = await reader.read();
				if (done) {
					// Allow the serial port to be closed later.
					reader.releaseLock();
					break;
				}
				if (value) {
					// console.log('received: ', (new TextDecoder()).decode(value));
				}
			}
		} catch (error) {
			// TODO: Handle non-fatal read error.
			console.log(error);
		}
	}
});

async function sendBoardUpdate() {
	if (writer === null)
		return;
	
	let data = new Uint8Array(9);
	data[0] = PACKETTYPE__STATE_UPDATE;
	for (let row = 0; row < 8; ++row) {
		let curByte = squares[row * 8].state;
		for (let col = 1; col < 8; ++col)
			curByte = (curByte << 1) | squares[row * 8 + col].state;
		data[row + 1] = curByte;
	}
	// console.log('sending: ', data);

	await writer.write(data);
	// writer.releaseLock();
}


//////////////////////////////////////////////////////////////////////////////
// https://konvajs.org/docs/events/Keyboard_Events.html

let squares = [];

let stage = new Konva.Stage({
	container: 'controlPanel',
	width: 800,
	height: 800,
  });
  squareWidth = (stage.width() - 10) / 8;
  squareHeight = (stage.height() - 10) / 8;

let layer = new Konva.Layer();

for (let i = 0; i < 64; ++i) {
	let rect = new Konva.Rect({
	  x: (i % 8) * (squareWidth + SQUARE_MARGIN) + SQUARE_MARGIN, y: Math.floor(i / 8) * (squareHeight + SQUARE_MARGIN) + SQUARE_MARGIN,
	  width: squareWidth, height: squareHeight
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

	rect.state = i < 16 || i > 47;
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
	});
	rect.on('mouseleave', function() {
        this.strokeWidth(0);
	});
	rect.on('click', function(evt) {
		console.log(evt);
		this.stateToggle();
		sendBoardUpdate();
	});

	squares.push(rect);
	layer.add(rect);
}

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