var abema_face = require('./build/Release/abema_face');
var $ = require('jquery');
var fs = require('fs');
var exec = require('child_process').exec;

var audio = new Audio('assets/camera.mp3');
audio.preload = 'auto';

$.fn.extend({
    animateCss: function (animationName) {
        var animationEnd = 'webkitAnimationEnd mozAnimationEnd MSAnimationEnd oanimationend animationend';
        this.addClass('animated ' + animationName).one(animationEnd, function() {
            $(this).removeClass('animated ' + animationName);
        });
    }
});

function flush() {
  audio.play();
  var wrapper = $('.flush');
  wrapper.css('z-index', 1000);
  wrapper.toggleClass('flush-active');
  setTimeout(() => {
    wrapper.css('z-index', 1);
    wrapper.toggleClass('flush-active', 100);
  });
}

function countdown() {
  var wrapper = $('.abema-wrapper');
  var elem = $('#pop');
  $('#thumb').attr('src', '');

  var count = 3;
  var decrement = () => {
    if (count == 0) {
      elem.text('cheese!');
      wrapper.animateCss('bounceIn');
      isEnable = false;
      abema_face.save('abema.png');
      flush();
      $('#thumb').attr('src', 'abema.png');
      setTimeout(() => {
        $('#dialog').toggleClass('shown');
        $('#name-input').focus();
        elem.text('face 2 abema');
        elem.removeClass('count');
      }, 1500);
    } else {
      elem.text(count);
      elem.addClass('count');
      wrapper.animateCss('bounceIn');
      count--;
      setTimeout(decrement, 1000);
    }
  };
  decrement();
}

isEnable = true;

document.addEventListener('DOMContentLoaded', function() {
  var canvas = document.getElementById('video');
  var context = canvas.getContext('2d');
  var width = canvas.clientWidth;
  var height = canvas.clientHeight;
  var imageData = context.createImageData(width, height);

  var update = function() {
    abema_face.draw(imageData.data, width, height);
    context.putImageData(imageData, 0, 0);
    if (isEnable) {
      setTimeout(update, 1);
    }
  };
  update();

  $(window).keydown((e) => {
    if (e.keyCode == 27 && isEnable) { // ESC key
      countdown();
    }
  });

  $('input').keypress(function(e){
    if(e.keyCode==13)
      $('#create-button').click();
  });

  $('#cancel-button').click(() => {
    $('#dialog').toggleClass('shown');
    isEnable = true;
    update();
  });

  $('#create-button').click(() => {
    name = $('#name-input').val();
    name2 = $('#name-input2').val();
    affiliate = $('#affiliate-input').val();
    args = name + ' ' + name2 + ' ' + affiliate;

    exec('python ../card/make_business_card.py ' + args, (error, stdout, stderr) => {
      $('#dialog').toggleClass('shown');
      $('#confirm-dialog').addClass('shown');
      setTimeout(() => {
        $('#confirm-dialog').removeClass('shown');
        isEnable = true;
        update();

        name = $('#name-input').val('');
        name2 = $('#name-input2').val('');
        affiliate = $('#affiliate-input').val('');
      }, 8000);
    });
  });
});

