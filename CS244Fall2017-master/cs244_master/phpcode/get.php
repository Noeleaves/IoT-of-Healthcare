<?php

echo 'Sparkfun POST data: ' . $_GET["name"];
file_put_contents("testRecord.txt", $_GET["name"]);

?>