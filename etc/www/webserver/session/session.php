<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Test des cookies</title>
    <link rel="stylesheet" href="./styles/style.css">
</head>
<body>
    <?php
        session_start();

        $usersFile = 'database';

        $users = [];
        if (file_exists($usersFile)) {
            $users = json_decode(file_get_contents($usersFile), true);
        }

        if (isset($_GET['logout'])) {
            session_destroy();
            header("Location: session.php");
            exit;
        }

        if (isset($_SESSION['username'])) {
            echo "<h1>Bienvenue, " . htmlspecialchars($_SESSION['username']) . "!</h1>";
            echo "<p><a href='?logout=1'>Déconnexion</a></p>";
        } else {
            if ($_SERVER['REQUEST_METHOD'] === 'POST') {
                $username = trim($_POST['username'] ?? '');
                $password = trim($_POST['password'] ?? '');
                $action = $_POST['action'] ?? 'login';

                if ($username === '' || $password === '') {
                    echo "<p style='color:red;'>Veuillez remplir tous les champs.</p>";
                } elseif ($action === 'register') {
                    if (isset($users[$username])) {
                        echo "<p style='color:red;'>Nom d'utilisateur déjà pris.</p>";
                    } else {
                        $users[$username] = password_hash($password, PASSWORD_DEFAULT);
                        file_put_contents($usersFile, json_encode($users));
                        $_SESSION['username'] = $username;
                        header("Location: session.php");
                        exit;
                    }
                } elseif ($action === 'login') {
                    if (isset($users[$username]) && password_verify($password, $users[$username])) {
                        $_SESSION['username'] = $username;
                        header("Location: session.php");
                        exit;
                    } else {
                        echo "<p style='color:red;'>Identifiants invalides.</p>";
                    }
                }
            }

            echo '<form method="post">
                <label>Nom d\'utilisateur: <input type="text" name="username"></label><br>
                <label>Mot de passe: <input type="password" name="password"></label><br>
                <input type="radio" name="action" value="login" checked> Connexion
                <input type="radio" name="action" value="register"> Inscription<br><br>
                <button type="submit">Valider</button>
            </form>';
        }
        ?>
</body>
</html>
