var abema_face = require('./build/Release/abema_face');
var $ = require('jquery');
var fs = require('fs');

document.addEventListener('DOMContentLoaded', function() {
  var canvas = document.getElementById('video');
  var context = canvas.getContext('2d');
  var width = canvas.clientWidth;
  var height = canvas.clientHeight;
  var imageData = context.createImageData(width, height);

  var update = function() {
    abema_face.draw(imageData.data, width, height);
    context.putImageData(imageData, 0, 0);
    setTimeout(update, 100);
  };
  update();

  $(window).keydown((e) => {
    if (e.keyCode == 27) { // ESC key
      $('#dialog').toggleClass('shown');
    }
  });

  $('#create-button').click(() => {
    abema_face.save('abema.png');
  });
});

