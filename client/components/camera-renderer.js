class CameraRenderer {
  constructor() {
    this.canvas = document.getElementById('video');
    this.context = this.canvas.getContext('2d');
    this.width = this.canvas.clientWidth;
    this.height = this.canvas.clientHeight;
    this.imageData = this.context.createImageData(this.width, this.height);

    this.isEnabled = true;
  }

  update() {
    if (!this.isEnabled) return;

    abema_face.draw(this.imageData.data, this.width, this.height);
    this.context.putImageData(this.imageData, 0, 0);
    setTimeout(this.update.bind(this), 1);
  }

  enable() {
    this.isEnabled = true;
    this.update();
  }

  disable () {
    this.isEnabled = false;
  }
};

module.exports = CameraRenderer;
