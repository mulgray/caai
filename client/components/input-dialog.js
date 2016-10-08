class InputDialog {
  constructor() {
    this.dialog = $('#input-dialog');
    this.okButton = $('#create-button');
    this.cancelButton = $('#cancel-button');
    this.thumbnail = $('#thumb');
    this.familyNameInput = $('#family-name-input');
    this.firstNameInput = $('#first-name-input');
    this.jobPositionInput = $('#job-position-input');

    this.onOkPressed = null;
    this.onCancelPressed = null;

    this.bindListeners();
  }

  bindListeners() {
    $('input').keypress((e) => {
      if (e.keyCode == 13) { this.enterKeyPressed(); }
    });

    this.okButton.click(this.okButtonPressed.bind(this));
    this.cancelButton.click(this.cancelButtonPressed.bind(this));
  }

  show(imagePath) {
    this.thumbnail.attr('src', imagePath);
    this.familyNameInput.focus();
    this.dialog.addClass('shown');
  }

  hide() {
    this.dialog.removeClass('shown');
    this.thumbnail.attr('src', '');
  }

  resetInputs() {
    this.familyNameInput.val('');
    this.firstNameInput.val('');
    this.jobPositionInput.val('');
  }

  enterKeyPressed() {
    this.okButton.click();
  }

  okButtonPressed() {
    this.hide();
    this.okPressed(
      this.familyNameInput.val(),
      this.firstNameInput.val(),
      this.jobPositionInput.val());
  }

  cancelButtonPressed() {
    this.hide();
    this.cancelPressed();
  }
}

module.exports = InputDialog;
