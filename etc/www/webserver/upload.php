<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title> UPLOID WITH CGI </title>
    <link rel="stylesheet" href="/styles/verify.css">
</head>
<body>
    <?php
        try
        {
            require_once(__DIR__ . "/script_php/script_upload.php");
            if (uploadIsDone(__DIR__))
            {                
                echo "<script>alert('File is upload.')</script>";
            }
            else
            {

                echo "<script>alert('Can not upload this file.')</script>";
            }
        }
        catch (\Throwable $th)
        {
            echo "<script>alert('Can not upload this file.')</script>";
        }
    ?>
    <a href="index.html"><-</a>
</body>
</html>