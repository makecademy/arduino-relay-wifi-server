// Function to motor direction
$(document).ready(function() {

  $("#1").click(function() {
    $.get('/relay_control/digital/8/1');
  });

  $("#2").click(function() {
    $.get('/relay_control/digital/8/0');
  });

});