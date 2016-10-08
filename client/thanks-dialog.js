class ThanksDialog {
  constructor() {
    this.elem = $('#thanks-dialog');
  }

  show() {
    this.elem.addClass('shown');
  }

  hide() {
    this.elem.removeClass('shown');
  }
}

module.exports = ThanksDialog;
