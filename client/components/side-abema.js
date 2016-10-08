// helper for animate.css
$.fn.extend({
  animateCss: function (animationName) {
    className = 'animated ' + animationName;
    this.addClass(className).one('animationend', () => {
      $(this).removeClass(className);
    });
  }
});

class SideAbema {
  constructor() {
    this.wrapper = $('.side-abema-pop');
    this.inner = $('#pop-text');
    this.defaultText = 'face 2 abema';
    this.inner.text(this.defaultText);
  }

  setText(text) {
    this.inner.text(text);
    this.wrapper.animateCss('bounceIn');

    if (text.length < 3) {
      this.inner.addClass('count');
    } else {
      this.inner.removeClass('count');
    }
  }

  resetText() {
    this.setText(this.defaultText);
  }
}

module.exports = SideAbema;
