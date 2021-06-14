Il Capitolo 4 presenta le prime applicazioni che utilizzano PortAudio per effettuare semplici streaming da e verso la scheda audio.

Nello specifico, verranno presentati alcuni esempi relativi a:

1. Verifica_Risorse: per verificare quali API native e device sono presenti nel prorpio sistema, come descritto nel paragrafo 4.6.
2. Scelta_Device: per selezionare uno dei device disponibili nel prorpio sistema, come descritto nel paragrafo 4.7.
3. Generazione_Sinusoide: implementa un oscillatore sinusoidale utilizzando la funzione seno della libreria matematica, come descritto nel paragrafo 4.11.2.1.
4. Generazione_Sinusoide_wavetable: implementa un oscillatore sinusoidale utilizzando la sintesi tabellare, come descritto nel paragrafo 4.11.2.2.
5. Generazione_Sinusoide-Sawtooth: implementa un oscillatore di un'onda sinusoidale sovrapposta a un'onda a dente di sega, come descritto nel paragrafo 4.11.3.
6. Full-Duplex_Amplificato: implementa uno streaming bidirezionale (da scheda ad applicazione e viceversa) con un'amplificazione diversa per i diversi canali, come descritto nel paragrafo 4.11.4.
7. Stereo_Panning: implementa uno stereo panning, ovvero un programma che sposta virtualmente una sorgente sonora dalla sinistra dell'ascoltatore alla sua destra, come descritto nel paragrafo 4.11.5.
8. Oscillatore_Tabellare: implementa la sintesi tabellare di diverse forme d'onda generalizzando l'esempio 4, come descritto nel paragrafo 4.12.
