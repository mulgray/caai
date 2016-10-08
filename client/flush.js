class Flush {
  constructor() {
    this.audio = new Audio('../assets/camera.mp3');
    this.audio.preload = 'auto';
    this.elem = $('.flush');
  }

  run() {
    this.audio.play();
    this.elem.css('z-index', 1000);
    this.elem.addClass('flush-active');
    setTimeout(() => {
      this.elem.css('z-index', 1);
      this.elem.removeClass('flush-active');
    }, 100);
  }
}

module.exports = Flush;
