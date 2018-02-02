<?php
/*
$data=  '{
        "acceleration":[
                [0.04,0.02,0.97],
                [0.04,0.02,0.97],
                [0.04,0.02,0.97],
                [0.04,0.02,0.97],
                [0.04,0.02,0.97],
                [0.04,0.02,0.97],
                [0.04,0.02,0.97],
                [0.04,0.02,0.97],
                [0.04,0.02,0.97],
                [0.04,0.02,0.97]
                ]
        }';
*/


$dataDecode = json_decode($_POST['data'],true);

if(file_exists ( "Lis3dh.csv" ))
    $file = fopen("Lis3dh.csv","a");
else
    $file = fopen("Lis3dh.csv","w");

foreach ($dataDecode['acceleration'] as $list)
{
    fputcsv($file,$list);
}

fclose($file);

echo "write CSV file complete.";

?>
