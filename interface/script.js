// Function to control relays
function relayClick(clicked_id){

    if (clicked_id == "1"){
        $.get( "curl.php", {
        command: "/digital/8/1"} );  
    } 

    if (clicked_id == "2"){
        $.get( "curl.php", {
        command: "/digital/8/0"} );  
    }
}