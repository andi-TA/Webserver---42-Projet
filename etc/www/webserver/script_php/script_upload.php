<?php
    function    uploadIsDone( $dir = __DIR__ ): bool
    {
        if (isset($_POST["Upload"]))
        {
            if (isset($_FILES["Content"]["name"]) && $_SERVER["DEPENDANCE"])
            {
                $dir = $dir . "/" .  $_SERVER["DEPENDANCE"] . "/";
                $target_file = $dir . basename($_FILES["Content"]["name"]);
                $size_file = $_FILES["Content"]["size"];
                if (!file_exists($target_file) && $size_file < 8000000)
                {
                    if (move_uploaded_file($_FILES["Content"]["tmp_name"], $target_file))
                        return (true);
                }
            }
        }
        return (false);
    }
?>
