const express = require('express');
const admin = require('firebase-admin');
const path = require('path'); // Importation du module 'path'
const app = express();

// Charger la clé Firebase depuis une variable d'environnement
const serviceAccount = JSON.parse(process.env.FIREBASE_SERVICE_ACCOUNT);

// Initialiser Firebase Admin SDK
admin.initializeApp({
  credential: admin.credential.cert(serviceAccount),
  databaseURL: "https://ciblerie-2-0.firebaseio.com" // Remplacez par votre URL Firebase
});

const db = admin.firestore();

// Servir les fichiers statiques (comme index.html) depuis le dossier 'public'
app.use(express.static(path.join(__dirname, 'public')));

// Route pour la racine
app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, 'public', 'index.html')); // Servir le fichier index.html
});

// Endpoint pour récupérer les scores
app.get('/scores', async (req, res) => {
  try {
    const scoresSnapshot = await db.collection('scores').orderBy('timestamp', 'desc').get();
    const scores = scoresSnapshot.docs.map(doc => ({
      id: doc.id,
      ...doc.data()
    }));
    res.json(scores);
  } catch (error) {
    console.error('Erreur lors de la récupération des scores :', error);
    res.status(500).send('Erreur serveur');
  }
});

// Endpoint pour vérifier le statut du serveur
app.get('/health', (req, res) => {
  res.send('Le serveur fonctionne correctement.');
});

// Démarrer le serveur
const PORT = process.env.PORT || 10000;
app.listen(PORT, () => {
  console.log(`Server is running on port ${PORT}`);
});