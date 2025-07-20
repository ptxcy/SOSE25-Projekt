const express = require('express');
const path = require('path');

const app = express();
const PORT = 80;

app.use(express.static(path.join(__dirname, '../../client/doc')));

app.get('/client', (req, res) => {
    res.sendFile(path.join(__dirname, '../../client/doc/doxygen-doc/html/index.html'));
});

app.listen(PORT, () => {
    console.log(`Info-Site läuft unter http://localhost:${PORT}`);
});
