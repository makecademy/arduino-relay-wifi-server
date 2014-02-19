<?php

    // Handle GET request
    $relayState = $_GET["relayState"];

    // Create a TCP/IP socket & connect to the server
    $socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
    $result = socket_connect($socket, "relay.local", "8888");

    // Request
    $in = "HEAD / HTTP/1.1\r\n";
    $in .= "Content-Type: text/html\r\n";
    $in .= $relayState . "\r\n\r\n";
    $out = '';

    // Send request
    socket_write($socket, $in, strlen($in));

    // Read answer
    while ($out = socket_read($socket, 4096)) {
        echo $out;
    }

    // Close socket
    socket_close($socket);

?>