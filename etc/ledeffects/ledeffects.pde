final int MATRIX_WIDTH = 8;
final int MATRIX_HEIGHT = 8;
final int CELL_SIZE = 64;
final int CELL_PADDING = 1;

class RGB {
  public int _r = 0;
  public int _g = 0;
  public int _b = 0;

  RGB() { }
  RGB(int r, int g, int b) { init(r, g, b); }
  RGB(RGB led) {
    init(led._r, led._g, led._b);
  }

  void init(int r, int g, int b) {
    _r = r;
    _g = g;
    _b = b;
  }
};

class CellRGB {
  public RGB[] _rgbs;
  public PVector _pos;
  public color _color = 0xffffff;

  CellRGB() { this(new PVector(), new RGB()); }
  CellRGB(PVector pos) { this(pos, new RGB()); }
  CellRGB(PVector pos, RGB led) { this(pos, led, #ffffff); }
  CellRGB(PVector pos, RGB led, color clr) {
    _rgbs = new RGB[2];
    for (int i = 0; i < 2; ++i)
      _rgbs[i] = new RGB(led);
    _pos = pos;
    _color = clr;
  }

  void draw() {
    fill(_color);
    noStroke();
    rect(_pos.x * (CELL_SIZE + CELL_PADDING) - CELL_SIZE / 2 + CELL_PADDING + CELL_SIZE / 2,
         _pos.y * (CELL_SIZE + CELL_PADDING) - CELL_SIZE / 2 + CELL_PADDING + CELL_SIZE / 2,
         CELL_SIZE, CELL_SIZE);
  }
};

class Matrix {
  public CellRGB[] cells;

  Matrix() {
    cells = new CellRGB[MATRIX_WIDTH * MATRIX_HEIGHT];
    for (int i = 0; i < MATRIX_WIDTH * MATRIX_HEIGHT; ++i) {
      cells[i] = new CellRGB(new PVector(i % MATRIX_WIDTH, (int)(i / MATRIX_HEIGHT)));
    }
  }

  void draw() {
    for (int i = 0; i < MATRIX_WIDTH * MATRIX_HEIGHT; ++i) {
      cells[i].draw();
    }
  }
};

Matrix matrix;

void settings() {
  size(MATRIX_WIDTH * (CELL_SIZE + CELL_PADDING) + CELL_PADDING,
       MATRIX_HEIGHT * (CELL_SIZE + CELL_PADDING) + CELL_PADDING);
}

void setup() {
  background(150);
  
  matrix = new Matrix();
}

void draw() {
  matrix.draw();
}