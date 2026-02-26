<?php
    $cookie_name = "test_cookie";
    $cookie_value = "cookie_ok";

    if (!isset($_COOKIE[$cookie_name])) {
        setcookie($cookie_name, $cookie_value);
        $cookie_set = false;
    } else {
        $cookie_set = true;
    }
?>
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Test des cookies</title>
    <link rel="stylesheet" href="./styles/style.css">
</head>
<body>
    <h1>Test support des cookies</h1>

    <?php if ($cookie_set): ?>
        <h3 style="color: green;">✅ Cookies supportés. Cookie reçu : <strong><?php echo $_COOKIE[$cookie_name]; ?></strong></h3>
    <?php else: ?>
        <h3 style="color: orange;">⚠️ Cookie défini, mais pas encore reçu.</h3>
    <?php endif; ?>
</body>
</html>
