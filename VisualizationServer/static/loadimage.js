$(document).ready( function() { 
	$('#image').html('<img src="/image"/>')
});

window.setInterval(function(){
   $('#image').html('<img src="/image"/>')
}, 5000);
