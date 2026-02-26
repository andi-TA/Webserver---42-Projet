<!DOCTYPE html>
<html lang="fr">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Présentation du Groupe</title>
    <link rel="stylesheet" href="/styles/about.css">
</head>

<body>
    <header>
        <h1>Notre Groupe</h1>
    </header>

    <section class="team">
        <div class="member">
            <img src="/attachments/nherimam.jpg" alt="jpg">
            <h2>nherimam</h2>
            <p>Responsable des traitements de requete du client et du cookies, session.</p>
        </div>
        <div class="member">
            <img src="/attachments/arazakar.jpg" alt="jpg">
            <h2>arazakar</h2>
            <p>Responsable du creation du server, configuration global et du liaison entre chaque composant.</p>
        </div>
        <div class="member">
            <img src="/attachments/atahiry-.jpg" alt="jpg">
            <h2>atahiry-</h2>
            <p>Responsable de l'envoie des reponses et du système de CGI, intégration PHP/Python et tests.</p>
        </div>
    </section>

    <section class="thanks">
        <h2>Merci a nous et surtout a vous pour nous evalue.</h2><br>
    </section>
    
    <footer>
        <?php
            if (isset($_GET["send"]) && isset($_GET["comment"]))
            {
                echo
                "<section class='com'>
                <h2> Guest comment : </h2>
                </p>" . $_GET["comment"] . "<p>
                </section><hr><br>";
            }
        ?>
        <h3>Laissez un commentaire</h3>
        <form method="GET">
            <textarea rows="3" cols="45" type="text" placeholder="Votre commentaire..." name="comment" id=""></textarea><br><br>
            <input type="submit" name="send">
        </form>
    </footer>
</body>

</html>