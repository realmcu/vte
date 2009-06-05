#!/bin/bash


1- se logger sur armageddon ou lescure ou vulcain ou venezia, ...
(pour voir les machines dispos, aller sur http://nemo.ea.freescale.net/ et cliquer sur "link")

2- aller dans sa vue 
2.1- Configurer sa vue pour accéder à la vob projet
        vob = prj_sw_tests
        Config Spec:
            element * /main/LATEST
            element * /main/0

2.2- Accéder à sa vue
    exemple:
        $> ct setview vw_apsw_svan01c
    
3- var d'env à modifier:
        $> setenv PATH /_TOOLS_/wrap/bin:${PATH}
        
4-  Attention:
        le fichier admin/delivery/xxx DOIT etre en check-in (xxx = VTE ou JET_ENV).
        le répertoire delivery/xxx_baselines peut rester en check-in (xxx = vte ou jet_env).
    
5- livraison
    exemple #1
        $> uptool mkdelivery -p sw_tests VTE_2.0 VTE
                Enter value for "VTEversion": 2.0
                Enter value for "LTPversion": 20060105
    
    exemple #2
        $> uptool mkdelivery -p sw_tests JET_2.0 JET
                Enter value for "JETversion": 2.0

6-  lock
    attention, une fois verrouillée, la livraison ne peut plus être déverrouillée.
        /* pour vérifier le status des livraisons */
        $> uptool lsdelivery -project sw_tests -long
        
        
        /* pour verrouiller une livraison */
        $> uptool lockdelivery -project sw_tests VTE_2.
