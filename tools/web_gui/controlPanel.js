const BAUD_RATE = 115200

///////////////////////////////////////////////////////////////////////////

const SQUARE_COLOR_INACTIVE = 'rgb(231, 231, 231)';
const SQUARE_COLOR_HOVER = 'rgb(196, 255, 191)';
const SQUARE_COLOR_CLICKED = 'rgb(255, 234, 118)';

const SQUARE_MARGIN = 1;


////////////////////////////////////////////////////////////////////////////

if ("serial" in navigator === false)
	alert("SerialPort isn't supported!!!");

document.querySelector('#btnConnect').addEventListener('click', async () => {
	const filters = [{usbVendorId: 0x1a86, usbProductId: 0x7523}];
	// const filters = [];
	const port = await navigator.serial.requestPort({filters});
	if (!port)
		return;
	await port.open({baudRate: BAUD_RATE});
	console.log('opened');

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
			  console.log(value);
			}
		  }
		} catch (error) {
		  // TODO: Handle non-fatal read error.
		  console.log(error);
		}
	  }
});


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


	rect.on('pointerover', function() {
        this.stroke('black');
        this.strokeWidth(2);
	});
	rect.on('pointerout', function() {
        this.strokeWidth(0);
	});
	rect.on('pointerclick', function() {
		this.stateToggle();
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