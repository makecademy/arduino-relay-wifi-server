function relayOn(){

	$.get( "update_state.php", {
		relayState: "On"} );
}

function relayOff(){

	$.get( "update_state.php", {
		relayState: "Off"} );
}