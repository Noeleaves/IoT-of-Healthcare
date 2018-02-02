<?php

echo 'Sparkfun POST data: ' . $_POST["name"];
file_put_contents("testRecord.txt", $_POST["name"]);

?>