<?php

    function deleteDir($dir): bool
    {
        if (!file_exists($dir))
            return (true);
        if (!is_dir($dir))
        {
            if (is_writable($dir))
                return (unlink($dir));
            return (false);
        }
        else
        {
            if (!is_writable(dirname($dir)))
                return (false);
        }
        foreach (scandir($dir) as $fil)
        {
            if ( $fil == "." || $fil == ".." )
                continue ;
            if (!deleteDir($dir . DIRECTORY_SEPARATOR . $fil))    
                return (false);
        }
        return rmdir($dir);
    }

    function deleteDone( $dir = __DIR__ ): bool
    {
        $pos = strpos($dir, "webserver");
        $val =  substr($dir, 0, $pos + strlen("webserver"));
        $fil =  $val . DIRECTORY_SEPARATOR . $_POST["fileDelete"];

        if (is_file($fil))
        {
            if (!file_exists($fil) || !is_writable($fil) || !is_readable($fil))
                return (false);
            return(unlink($fil));
        }
        else if (is_dir($fil))
        {
            if (!is_writable(dirname($fil)))
                return (false);
            return (deleteDir($fil));
        }
    }
?>