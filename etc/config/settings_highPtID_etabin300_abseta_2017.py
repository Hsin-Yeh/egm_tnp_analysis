#############################################################
########## General settings
#############################################################
# flag to be Tested
flags = {
    'passingCutBasedLooseHighPtIDV2_manualCut'                 : '( (ph_chIso<5 && corPhoIso<2.75 && ph_r9>0.8 && ph_sieie<0.0105 && ph_hTowOverE<0.05 && abs(ph_sc_eta)<1.4442) || (ph_chIso<5 && corPhoIso<2.00 && ph_r9>0.8 && ph_sieie<0.0280 && ph_hTowOverE<0.05 && abs(ph_sc_eta)<2.5 && abs(ph_sc_eta)>1.566) )',
    'passingCutBasedLooseHighPtIDV2'                           : '(passingCutBasedLooseHighPtIDV2 == 1)',
    'passingCutBasedLooseHighPtIDV2PhoAnyPFIsoCuthighPtID'     : '(passingCutBasedLooseHighPtIDV2PhoAnyPFIsoCuthighPtID == 1)',
    'passingCutBasedLooseHighPtIDV2PhoAnyPFIsoCuthighPtID1'    : '(passingCutBasedLooseHighPtIDV2PhoAnyPFIsoCuthighPtID1 == 1)',
    'passingCutBasedLooseHighPtIDV2PhoFull5x5R9Cut'            : '(passingCutBasedLooseHighPtIDV2PhoFull5x5R9Cut == 1)',
    'passingCutBasedLooseHighPtIDV2PhoFull5x5SigmaIEtaIEtaCut' : '(passingCutBasedLooseHighPtIDV2PhoFull5x5SigmaIEtaIEtaCut == 1)',
    'passingCutBasedLooseHighPtIDV2PhoSingleTowerHadOverEmCut' : '(passingCutBasedLooseHighPtIDV2PhoSingleTowerHadOverEmCut == 1)',
    }

baseOutDir = 'results/ReReco2017/tnpPhoID/etabin300_abseta/'

#############################################################
########## samples definition  - preparing the samples
#############################################################
### samples are defined in etc/inputs/tnpSampleDef.py
### not: you can setup another sampleDef File in inputs
import etc.inputs.tnpSampleDef as tnpSamples
tnpTreeDir = 'tnpPhoIDs'

samplesDef = {
    # 'data'   : tnpSamples.ExoReReco2017['data'].clone(),
    # 'mcNom'  : tnpSamples.ExoReReco2017['mc'].clone(),

    'data'   : tnpSamples.ExoReReco2017['data_Run2017B'].clone(),
    # 'mcNom'  : tnpSamples.ExoReReco2017['DY_madgraph'].clone(),
    'mcNom'  : tnpSamples.ExoReReco2017['DY_madgraph_HTbinned'].clone(),
    'mcAlt'  : tnpSamples.ExoReReco2017['DY_amcatnloext'].clone(),
    # 'tagSel' : tnpSamples.ExoReReco2017['DY_madgraph'].clone(),
    'tagSel' : tnpSamples.ExoReReco2017['DY_madgraph_HTbinned'].clone(),
}
## can add data sample easily
samplesDef['data'].add_sample( tnpSamples.ExoReReco2017['data_Run2017C'] )
samplesDef['data'].add_sample( tnpSamples.ExoReReco2017['data_Run2017D'] )
samplesDef['data'].add_sample( tnpSamples.ExoReReco2017['data_Run2017E'] )
samplesDef['data'].add_sample( tnpSamples.ExoReReco2017['data_Run2017F'] )

# samplesDef['mcNom'].add_sample( tnpSamples.ExoReReco2017['DY_madgraph_HT-100to200'] )
# samplesDef['mcNom'].add_sample( tnpSamples.ExoReReco2017['DY_madgraph_HT-200to400'] )
# samplesDef['mcNom'].add_sample( tnpSamples.ExoReReco2017['DY_madgraph_HT-400to600'] )
# samplesDef['mcNom'].add_sample( tnpSamples.ExoReReco2017['DY_madgraph_HT-600to800'] )
# samplesDef['mcNom'].add_sample( tnpSamples.ExoReReco2017['DY_madgraph_HT-800to1200'] )
# samplesDef['mcNom'].add_sample( tnpSamples.ExoReReco2017['DY_madgraph_HT-1200to2500'] )
# samplesDef['mcNom'].add_sample( tnpSamples.ExoReReco2017['DY_madgraph_HT-2500toInf'] )

# samplesDef['tagSel'].add_sample( tnpSamples.ExoReReco2017['DY_madgraph_HT-100to200'] )
# samplesDef['tagSel'].add_sample( tnpSamples.ExoReReco2017['DY_madgraph_HT-200to400'] )
# samplesDef['tagSel'].add_sample( tnpSamples.ExoReReco2017['DY_madgraph_HT-400to600'] )
# samplesDef['tagSel'].add_sample( tnpSamples.ExoReReco2017['DY_madgraph_HT-600to800'] )
# samplesDef['tagSel'].add_sample( tnpSamples.ExoReReco2017['DY_madgraph_HT-800to1200'] )
# samplesDef['tagSel'].add_sample( tnpSamples.ExoReReco2017['DY_madgraph_HT-1200to2500'] )
# samplesDef['tagSel'].add_sample( tnpSamples.ExoReReco2017['DY_madgraph_HT-2500toInf'] )

## some sample-based cuts... general cuts defined here after
## require mcTruth on MC DY samples and additional cuts
## all the samples MUST have different names (i.e. sample.name must be different for all)
## if you need to use 2 times the same sample, then rename the second one
#samplesDef['data'  ].set_cut('run >= 273726')
if not samplesDef['mcNom' ] is None: samplesDef['mcNom' ].set_mcTruth()
if not samplesDef['mcAlt' ] is None: samplesDef['mcAlt' ].set_mcTruth()
if not samplesDef['tagSel'] is None: samplesDef['tagSel'].set_mcTruth()
if not samplesDef['tagSel'] is None:
    samplesDef['tagSel'].rename('mcAltSel_DY_madgraph')
    samplesDef['tagSel'].set_cut('tag_Ele_pt > 37 && tag_Ele_passmedium_V1')

samplesDef['data' ].set_tnpTree(tnpTreeDir)
if not samplesDef['mcNom' ] is None: samplesDef['mcNom' ].set_tnpTree(tnpTreeDir)
if not samplesDef['mcAlt' ] is None: samplesDef['mcAlt' ].set_tnpTree(tnpTreeDir)
if not samplesDef['tagSel'] is None: samplesDef['tagSel'].set_tnpTree(tnpTreeDir)

## set MC weight, simple way (use tree weight)
weightName = 'xsecWeight'
if not samplesDef['mcNom' ] is None: samplesDef['mcNom' ].set_weight(weightName)
# if not samplesDef['mcAlt' ] is None: samplesDef['mcAlt' ].set_weight(weightName)
if not samplesDef['tagSel'] is None: samplesDef['tagSel'].set_weight(weightName)

# weightName = 'weights_2017_run2017.totWeight'
# if not samplesDef['mcNom' ] is None: samplesDef['mcNom' ].set_weight(weightName)
# if not samplesDef['mcAlt' ] is None: samplesDef['mcAlt' ].set_weight(weightName)
# if not samplesDef['tagSel'] is None: samplesDef['tagSel'].set_weight(weightName)
# if not samplesDef['mcNom' ] is None: samplesDef['mcNom' ].set_puTree('/eos/home-h/hsinyeh/store/Crab1022_test/PU/DY_madgraph_pho.pu.puTree.root')
# if not samplesDef['mcAlt' ] is None: samplesDef['mcAlt' ].set_puTree('/eos/home-h/hsinyeh/store/Crab1022_test/PU/DY_amcatnlo_pho.pu.puTree.root')
# if not samplesDef['tagSel'] is None: samplesDef['tagSel'].set_puTree('/eos/home-h/hsinyeh/store/Crab1022_test/PU/DY_madgraph_pho.pu.puTree.root')


#############################################################
########## bining definition  [can be nD bining]
#############################################################

biningDef = [
    { 'var' : 'abs(ph_sc_eta)' , 'type': 'float', 'bins': [0.0, 0.8, 1.566, 2.5] },
    # { 'var' : 'ph_et' , 'type': 'float', 'bins': [125,200,300,400,500,1000] },
    { 'var' : 'ph_et' , 'type': 'float', 'bins': [125, 200, 300, 1000] },
]

#############################################################
########## Cuts definition for all samples
#############################################################
### cut
cutBase   = 'tag_Ele_pt > 35 && abs(tag_sc_eta) < 2.1 && tag_Ele_passtight_V1'

# can add addtionnal cuts for some bins (first check bin number using tnpEGM --checkBins)
additionalCuts = {
}

#### or remove any additional cut (default)
additionalCuts = None

#############################################################
########## fitting params to tune fit by hand if necessary
#############################################################
tnpParNomFit = [
    "meanP[-0.0,-5.0,5.0]","sigmaP[0.9,0.5,5.0]",
    "meanF[-0.0,-5.0,5.0]","sigmaF[0.9,0.5,5.0]",
    "acmsP[60.,50.,90.]","betaP[0.04,0.01,0.08]","gammaP[0.1, -2, 2]","peakP[90.0]",
    "acmsF[60.,50.,90.]","betaF[0.04,0.01,0.08]","gammaF[0.1, -2, 2]","peakF[90.0]",
    ]

tnpParAltSigFit = [
    "meanP[-0.0,-5.0,5.0]","sigmaP[1,0.7,6.0]","alphaP[2.0,1.2,3.5]" ,'nP[3,-5,5]',"sigmaP_2[1.5,0.5,6.0]","sosP[1.3,0.5,5.0]",
    "meanF[-0.0,-5.0,5.0]","sigmaF[2,0.7,15.0]","alphaF[2.0,1.2,3.5]",'nF[3,-5,5]',"sigmaF_2[2.0,0.5,6.0]","sosF[1.3,0.5,5.0]",
    "acmsP[50.,40.,80.]","betaP[0.03,0.01,0.06]","gammaP[0.1, 0.005, 1]","peakP[90.0,85,95]",
    "acmsF[50.,40.,80.]","betaF[0.03,0.01,0.06]","gammaF[0.1, 0.005, 1]","peakF[90.0,85,95]",
    ]

# tnpParAltBkgFit = [
#     "meanP[1.19,-5.0,5.0]","sigmaP[0.74,0.5,5.0]",
#     "meanF[1.03,-5.0,5.0]","sigmaF[2.22,0.5,5.0]",
#     "alphaP[0.,-5.,5.]",
#     "alphaF[0.,-5.,5.]",
#     ]


# For Parabolic bkg fit
tnpParAltBkgFit = [
    "meanP[1.19,-5.0,5.0]","sigmaP[0.74,0.5,5.0]",
    "meanF[0.9,-5.0,5.0]","sigmaF[1.2,0.5,5.0]",
    "linearP[1,0.001,10]","parabolicP[0,-1,1]",
    "linearF[0.3,-1,10]","parabolicF[0,-5,5]",
    ]
