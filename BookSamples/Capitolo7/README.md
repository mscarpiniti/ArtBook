Il Capitolo 7 fornisce una rapida panoramica sui fondamenti dell'elaborazione numerica dei segnali (o DSP - Digital Signal Processing) con particolare enfasi alle applicazioni audio.

Nello specifico, sono mostrati esempi relativi all'implementazione di filtri FIR, IIR, comb, all-pass e tecniche più specifiche come l'overlap and add e l'overlap and save:

1. Convoluzione: implementa la somma di convoluzione, descritta nel paragrafo 7.3.
2. Filtro_FIR: implementa un filtro FIR, descritto nel paragrafo 7.5.1.
3. Filtro_IIR: implementa un filtro IIR, descritto nel paragrafo 7.5.2.
4. Filtro_I_ordine: implementa un filtro passa-basso/alto con strutture del I ordine, descritto nel paragrafo 7.6.1.
5. Filtro_II_ordine: implementa un filtro passa/arresta-banda con strutture del II ordine, descritto nel paragrafo 7.6.2.
6. Filtro_Shelving_I_ordine: implementa un filtro shelving passa-basso/alto del I ordine, descritto nel paragrafo 7.9.
7. Filtro_Shelving_II_ordine: implementa un filtro shelving peak/notch del II ordine, descritto nel paragrafo 7.9.
8. Ritardo_BufferLineare: implementa un ritardo con la tecnica del buffer lineare, come descritto nel paragrafo 7.11.1.
9. Ritardo_BufferCircolare: implementa un ritardo con la tecnica del buffer circolare, come descritto nel paragrafo 7.11.1.
10. Filtro_comb: implementa un filtro comb ricorsivo con la tecnica del buffer circolare, come descritto nel paragrafo 7.11.2.
11. Filtro_allpass: implementa un filtro all-pass con la tecnica del buffer circolare, come descritto nel paragrafo 7.11.2.
12. Ritardo_Frazionario: implementa un ritardo frazionario con la tecnica del buffer circolare e l'interpolazione lineare, come descritto nel paragrafo 7.12.
13. Overlap_and_Add: implementa la tecnica di filtraggio in frequenza con il metodo dell'overlap and add (OLA), come descritto nel paragrafo 7.13.1.
14. Overlap_and_Save: implementa la tecnica di filtraggio in frequenza con il metodo dell'overlap and save (OLS), come descritto nel paragrafo 7.13.2.
15. Rumore_Bianco: implementa la generazione di rumore bianco, come descritto nel paragrafo 7.14.
