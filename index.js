const express = require('express');
const admin = require('firebase-admin');
const app = express();

// Lire la clé Firebase depuis une variable d'environnement
const serviceAccount = JSON.parse(process.env.FIREBASE_SERVICE_ACCOUNT);

admin.initializeApp({
  credential: admin.credential.cert(serviceAccount),
  databaseURL: "https://ciblerie-2-0.firebaseio.com" // Remplacez par votre URL Firebase
});

const db = admin.firestore();

// Démarrer le serveur
const PORT = process.env.PORT || 10000;
app.listen(PORT, () => {
  console.log(`Server is running on port ${PORT}`);
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
