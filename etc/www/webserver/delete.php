<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Webserver - Delete File</title>
  <link rel="stylesheet" href="/styles/delete.css">
</head>
<body>
  <h1>Delete a File</h1>
  <?php
    try
    {
      if (isset($_POST["Del"]))
      {
        require_once(__DIR__ . "/script_php/script_delete.php");
        if (deleteDone(__DIR__))
            echo "<script>alert('Element is del.')</script>";
        else
            echo "<script>alert('Element can\'t del.')</script>";
      }
    }
    catch(\Throwable $th)
    {
        echo "<script>alert('Element can\'t del.')</script>";
    }
  ?>
  <form method="POST" enctype="application/x-www-form-urlencoded">
    <input type="text" name="delete" value="del" hidden>
    <label for="fileDelete">File to delete :</label>
    <input type="text" id="fileDelete" name="fileDelete" placeholder="Enter filename" required>
    <input type="submit" name="Del">
  </form>
</body>
</html>
