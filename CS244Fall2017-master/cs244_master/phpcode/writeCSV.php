<?php
/*
$json = '{
          "data": [
                [12345,54321],
                [12345,54321],
                [12345,54321],
                [12345,54321]
          ],
          "powerLevel": 1
        }';
*/

$dataDecode = json_decode($_POST['data'],true);

if(file_exists ( "max30105-p".$dataDecode[powerLevel].".csv" ))
    $file = fopen("max30105-p".$dataDecode[powerLevel].".csv","a");
else
    $file = fopen("max30105-p".$dataDecode[powerLevel].".csv","w");

foreach ($dataDecode['data'] as $list)
{
    fputcsv($file,$list);
}

fclose($file);

echo "write CSV file complete.";

?>
