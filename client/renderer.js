var abema_face = require('../build/Release/face2abema');
var $ = require('jquery');
var fs = require('fs');
var exec = require('child_process').exec;

var SideAbema = require('./components/side-abema');
var InputDialog = require('./components/input-dialog');
var ThanksDialog = require('./components/thanks-dialog');
var Flush = require('./components/flush');
var CameraRenderer = require('./components/camera-renderer');

// helper function for promise chain
function wait(milliseconds) {
  return new Promise((resolve, reject) => {
    setTimeout(() => { return resolve(); }, milliseconds);
  });
};

class Face2Abema {
  constructor() {
    // view components
    this.sideAbema = new SideAbema();
    this.inputDialog = new InputDialog();
    this.thanksDialog = new ThanksDialog();
    this.flush = new Flush();
    this.renderer = new CameraRenderer();

    // events
    this.inputDialog.okPressed = (...args) =>
      this.inputDialogOkPressed(...args);
    this.inputDialog.cancelPressed = () =>
      this.inputDialogCancelPressed();
  }

  inputDialogOkPressed(first, family, position) {
    this.createCard(first, family, position);
    this.thanksDialog.show();
    wait(8000)
      .then(() => {
        this.thanksDialog.hide();
        this.renderer.enable();
      });
  }

  inputDialogCancelPressed() {
    this.thanksDialog.hide();
    this.renderer.enable();
  }

  run() {
    this.renderer.update();
  }

  shot() {
    this.sideAbema.setText('3');
    wait(1000)
      .then(() => this.sideAbema.setText('2'))
      .then(() => wait(1000))
      .then(() => this.sideAbema.setText('1'))
      .then(() => wait(1000))
      .then(() => {
        this.sideAbema.setText('cheese!');
        abema_face.save('abema.png');
        this.flush.run();
        this.renderer.disable();
      })
      .then(() => wait(1500))
      .then(() => {
        this.inputDialog.show('../abema.png');
        this.sideAbema.resetText();
      });
  }

  createCard(family_name, first_name, job_position)  {
    const command = [
      'python card/make_business_card.py',
      family_name,
      first_name,
      job_position
    ].join(' ');

    return new Promise((resolve, reject) => {
      exec(command, (error, stdout, stderr) => {
        // TODO: handle error
        resolve();
      });
    });
  }
}

var face2Abema = null;

document.addEventListener('DOMContentLoaded', () => {
  face2Abema = new Face2Abema();
  face2Abema.run();

  // ESC key to take a shot
  $(window).keydown((e) => {
    if (e.keyCode == 27) face2Abema.shot();
  });
});
