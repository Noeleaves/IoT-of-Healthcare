<?php
/*
$data=  '{
        "sensor":[
                [0.04,0.02,0.97,3123213,123213213],
                [0.04,0.02,0.97,3123213,123213213],
                [0.04,0.02,0.97,3123213,123213213]
                ]
        }';
*/


$dataDecode = json_decode($_POST['data'],true);

if(file_exists ( "sensor.csv" ))
    $file = fopen("sensor.csv","a");
else
    $file = fopen("sensor.csv","w");

foreach ($dataDecode['sensor'] as $list)
{
    fputcsv($file,$list);
}

fclose($file);

echo "write CSV file complete.";

?>
