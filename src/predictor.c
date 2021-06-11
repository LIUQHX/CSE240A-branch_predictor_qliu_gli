//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include "predictor.h"
#include <string.h>

//
// TODO:Student Information
//
const char *studentName = "Qihong Liu; Guanghao Li";
const char *studentID   = "A59004984; A59002482";
const char *email       = "qil011@ucsd.edu; ";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
                          "Tournament", "Custom" };

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

// tournament
uint32_t *lht;  // local history table
uint8_t *lbt;  // local branch table
uint32_t ghr;  // global history register
uint8_t *gbt;  // global branch table
uint8_t *cbt;  // choice branch table

uint8_t gPredition; // global branch prediction
uint8_t lPredition; // local branch prediction
uint8_t tPredition; // tournament branch prediction

// gshare
uint32_t globalHistory;
uint8_t *globalBHT;


//
//TODO: Add your own Branch Predictor data structures here
//
// perceptron
/*
uint8_t pPrediction;
uint32_t perceptron_num;
uint32_t **weights;
uint32_t weight_num;
uint8_t *history;
uint32_t threshold;
uint32_t history_len;
*/
uint32_t nbits;
int **weights;
uint32_t nweights;
uint32_t nperceptrons;
uint32_t histlength;
int8_t *history;
uint32_t threshold;
uint32_t predictor_size;
uint32_t pcmask;

uint32_t
make_mask(uint32_t size)
{
  uint32_t mmask = 0;
  for(int i=0;i<size;i++)
  {
    mmask = mmask | 1<<i;
  }
  return mmask;
}


//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

//------------------------------------//
//              Gshare                //
//------------------------------------//

void init_gshare_predictor(){
	// init globalHistory and global Branch History Table
	globalHistory = 0;
	int bitsNeed = sizeof(uint8_t) * (1<< pcIndexBits);
	globalBHT = malloc(sizeof(uint8_t) * (1<< pcIndexBits));
	if(globalBHT==NULL){
		fprintf(stderr, "[ERROR] [Malloc] global branch history table failed.\n");
	    exit(-1);
	}
	memset(globalBHT, WN, sizeof(uint8_t) * (1<< pcIndexBits));
	printf("Gshare initialization finished.");
}

uint8_t make_gshare_prediction(uint32_t pc){
	// compute index
	uint32_t bitMask = (1 << ghistoryBits) - 1;
	uint32_t globalBHT_index = (pc ^ globalHistory) & bitMask;
	// get prediction from globalBHT
	if(globalBHT[globalBHT_index] < WT) return NOTTAKEN;
	else return TAKEN;
}

void train_gshare_predictor(uint32_t pc, uint8_t outcome){
	// compute index
	uint32_t bitMask = (1 << ghistoryBits) - 1;
	//uint32_t globalBHT_index = (pc ^ globalHistory) & bitMask;
  uint32_t globalBHT_index = (globalHistory ^ pc) & bitMask;
	// 2-bit predict
	if(outcome == TAKEN){
		if(globalBHT[globalBHT_index] < ST){
      globalBHT[globalBHT_index] ++;
    }
	}
	else{
		if(globalBHT[globalBHT_index] > SN){
      globalBHT[globalBHT_index] --;
    }
	}
  // update globalHistory
  globalHistory = (globalHistory << 1) | outcome;
}

//------------------------------------//
//             Tournament             //
//------------------------------------//

void init_tournament_predictor(){
  // Initialize Local History Table
  lht=(uint32_t*)malloc((1<<pcIndexBits)*sizeof(uint32_t));
  if(lht==NULL){
    fprintf(stderr, "[ERROR] [Malloc] local history table failed.\n");
    exit(-1);
  }
  memset(lht, 0, (1<<pcIndexBits)*sizeof(uint8_t));

  // Initialize Local Branch Table
  lbt=(uint8_t*)malloc((1<<lhistoryBits)*sizeof(uint8_t));
  if(lbt==NULL){
    fprintf(stderr, "[ERROR] [Malloc] local branch table failed.\n");
    exit(-1);
  }
  memset(lbt, WN, (1<<lhistoryBits)*sizeof(uint8_t));

  // Initialize Global History Register
  ghr = 0;

  // Initialize Global Branch Table
  gbt=(uint8_t*)malloc((1<<ghistoryBits)*sizeof(uint8_t));
  if(gbt==NULL){
    fprintf(stderr, "[ERROR] [Malloc] global branch table failed.\n");
    exit(-1);
  }
  memset(gbt, WN, (1<<ghistoryBits)*sizeof(uint8_t));

  // Initialize Choice Branch Table
  cbt=(uint8_t*)malloc((1<<ghistoryBits)*sizeof(uint8_t));
  if(cbt==NULL){
    fprintf(stderr, "[ERROR] [Malloc] choice branch table failed.\n");
    exit(-1);
  }
  memset(cbt, WN, (1<<ghistoryBits)*sizeof(uint8_t));

  gPredition=WN;
  lPredition=WN;

  //printf("tournament initialization finished.");
}

uint8_t make_local_prediction(uint32_t pc){
  uint32_t lbtPred=lbt[lht[pc & ((1<<pcIndexBits) -1)]];
  if(lbtPred == WN || lbtPred == SN){
    return NOTTAKEN;
  }
  else{
    return TAKEN;
  }
}

uint8_t make_global_prediction(uint32_t pc){
  uint32_t gbtPred=gbt[ghr & ((1<<ghistoryBits) -1)];
  if(gbtPred == WN || gbtPred == SN){
    return NOTTAKEN;
  }
  else{
    return TAKEN;
  }
}
uint8_t tournament_choose(uint8_t localPred, uint8_t globalPred, uint32_t pc){
  uint8_t choice = cbt[ghr & ((1<<ghistoryBits)-1)];
  return (choice == WN || choice == SN) ? globalPred : localPred;
}

uint8_t make_tournament_prediction(uint32_t pc){
  uint8_t localPred = make_local_prediction(pc);
  uint8_t globalPred = make_global_prediction(pc);
  return tournament_choose(localPred, globalPred, pc);
}

void train_local_predictor(uint32_t pc, uint8_t outcome) {
  // update lbt
  if(outcome==NOTTAKEN){
    if(lbt[lht[pc & ((1<<pcIndexBits) -1)]]!=SN){
      lbt[lht[pc & ((1<<pcIndexBits) -1)]]--;
    }
  }
  else{
    if(lbt[lht[pc & ((1<<pcIndexBits) -1)]]!=ST){
      lbt[lht[pc & ((1<<pcIndexBits) -1)]]++;
    }
  }

  // update lht
  lht[pc & ((1<<pcIndexBits) -1)] <<=1 ;
  lht[pc & ((1<<pcIndexBits) -1)] &= ((1 << lhistoryBits) - 1);
  lht[pc & ((1<<pcIndexBits) -1)] |= outcome;

}

void train_global_predictor(uint32_t pc, uint8_t outcome) {
  // update gbt
  if(outcome==NOTTAKEN){
    if(gbt[ghr & ((1<<ghistoryBits) -1)]!=SN){
      gbt[ghr & ((1<<ghistoryBits) -1)]--;
    }
  }
  else{
    if(gbt[ghr & ((1<<ghistoryBits) -1)]!=ST){
      gbt[ghr & ((1<<ghistoryBits) -1)]++;
    }
  }
  // update ghr
  ghr <<=1;
  ghr &= ((1 << ghistoryBits) - 1);
  ghr |= outcome;
}

void train_chooser_predictor(uint32_t pc, uint8_t outcome) {
  uint8_t localOutcome = make_local_prediction(pc);
  uint8_t globalOutcome = make_global_prediction(pc);

  uint32_t lbtPred=lbt[lht[pc & ((1<<pcIndexBits) -1)]];
  uint32_t gbtPred=gbt[ghr & ((1<<ghistoryBits) -1)];
  uint8_t choice = cbt[ghr & ((1<<ghistoryBits)-1)];

  if(localOutcome!=globalOutcome){
    if(localOutcome==outcome && choice!=ST){
      cbt[ghr & ((1<<ghistoryBits)-1)]++;
    }
    else if(globalOutcome==outcome && choice!=SN){
      cbt[ghr & ((1<<ghistoryBits)-1)]--;
    }
  }
}

void train_tournament_predictor(uint32_t pc, uint8_t outcome){
  // train chooser
  train_chooser_predictor(pc, outcome);
  // train local predictor
  train_local_predictor(pc, outcome);
  // train global predictor
  train_global_predictor(pc, outcome);
}

//------------------------------------//
//               Custom               //
//------------------------------------//

void init_custom_predictor(){
  /*
	history_len = 128;
	perceptron_num = 413436;
	weight_num = history_len + 1;
	threshold = 1.93*history_len+14;
	// init global history for perceptron prediction
	int bitNeed = history_len*sizeof(uint8_t);
	history = (uint8_t*)malloc(bitNeed);
	if(history==NULL){
		fprintf(stderr, "[ERROR] [Malloc] history failed.\n");
	    exit(-1);
	}
	memset(history, 0, bitNeed);
	// init weights matrix (perceptronNumber, weightsNumber)
	weights = (uint32_t**)malloc(sizeof(uint32_t*) * perceptron_num);
	if(weights==NULL){
		fprintf(stderr, "[ERROR] [Malloc] weights failed.\n");
	    exit(-1);
	}
	for(int i=0; i<perceptron_num; i++){
		weights[i] = (uint32_t*)malloc(sizeof(uint32_t) * weight_num);
		if(weights[i]==NULL){
			fprintf(stderr, "[ERROR] [Malloc] weights[i] failed.\n");
		    exit(-1);
		}
		memset(weights[i], 0, sizeof(uint32_t) * weight_num); 
	}
  */
  int size;
  //ghist = 0;
  //gmask = (1<<ghistoryBits -1);
  //lmask = (1<<lhistoryBits -1);

  nbits = 12;
  predictor_size = 640000000;
  histlength = 128;
  pcmask = make_mask(nbits);
  nweights = histlength+1; // inputs + 1 bias
  threshold = 1.93 * histlength + 14;
  nperceptrons = (int)(predictor_size/(nweights*nbits)); // size / (nbits bits per weight * nweights)
  weights = (int**) malloc(sizeof(int*)*nperceptrons);
  for(int i=0;i<nperceptrons;i++)
    weights[i] = (int*) malloc(sizeof(int)*nweights);

  for(int i=0;i<nperceptrons;i++)
  {
    for(int j=0;j<nweights;j++)
    {
      weights[i][j] = 0;
    }
  }
  history = (int8_t*) malloc(sizeof(int8_t)*histlength);
  for(int i=0;i<histlength;i++)
    history[i] = 0;
}

uint32_t y;
uint8_t make_custom_prediction(uint32_t pc){
  int ppred = 0;
  uint32_t idx;
  int hist;
  /*
	// compute index
	uint32_t percIdx=(pc & ((1<<pcIndexBits)-1)) % perceptron_num;
	// calculate prediction y
	y = 0;
	for(int i=0;i<=history_len;i++){
		if(i != history_len) y += (history[i]>0)?weights[percIdx][i]:(-weights[percIdx][i]);
		else y += weights[percIdx][i];
	}
	if(y>=0) return TAKEN;
	else return NOTTAKEN;
  */
  idx = (pc & pcmask) % nperceptrons;
  for(int i=0;i<histlength;i++)
  {
    hist = (history[i]>=0) ? 1:-1;
    ppred += weights[idx][i] * hist;
  }
  ppred += weights[idx][histlength];
  if(ppred>=0)
    return TAKEN;
  else
    return NOTTAKEN;
}

void train_custom_predictor(uint32_t pc, uint8_t outcome){
  //printf("hi");
  //uint32_t percIdx=(pc & ((1<<pcIndexBits)-1)) % perceptron_num;
  /*
  int y=0;
  for(int i=0;i<weight_num;i++){
    y+=(history[i]>0)?weights[percIdx][i]:(-weights[percIdx][i]);
  }
  */
  /*
  int out_sign=(outcome==TAKEN)?1:-1;
  int y_sign=(y>=0)?1:-1;
  if(y_sign==out_sign || abs(y)<=threshold){
    for(int i=0;i<history_len;i++){
      //uint32_t
      weights[percIdx][i]=weights[percIdx][i]+(history[i]>=0?out_sign:(-out_sign));
    }
    weights[percIdx][history_len]=out_sign;
  }
  for(int i=0;i<history_len-1;i++){
    history[i]=history[i+1];
  }
  history[history_len-1]=outcome;
  return;
  */
  int ppred = 0;
  uint32_t idx;
  int hist;
  int out;
  uint8_t prediction = make_prediction(pc);
  
  idx = (pc & pcmask) % nperceptrons;
  for(int i=0;i<nweights;i++)
  {
    hist = (history[i]>=0) ? 1:-1;
    ppred += weights[idx][i] * hist;
  }
  if(prediction!=outcome || abs(ppred)<=threshold)
  {
    out = (outcome>0) ? 1:-1;
    for(int i=0;i<histlength;i++)
    {
      hist = (history[i]>=0) ? 1:-1;
      weights[idx][i] += out * hist;
    }
    weights[idx][histlength] = out;
  }
  for(int i=0;i<histlength-1;i++)
  {
    history[i] = history[i+1];
  }
  history[histlength-1] = outcome;
  return;

}
//------------------------------------//
//          Predictor Entry           //
//------------------------------------//

// Initialize the predictor
//
void
init_predictor()
{
  switch (bpType) {
    case STATIC:
      break;
    case GSHARE:
      init_gshare_predictor();
      break;
    case TOURNAMENT:
      init_tournament_predictor();
      break;
    case CUSTOM:
      init_custom_predictor();
      break;
    default:
      break;
  }
  return;
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
make_prediction(uint32_t pc)
{
  //
  //TODO: Implement prediction scheme
  //

  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
      return TAKEN;
    case GSHARE:
      return make_gshare_prediction(pc);
    case TOURNAMENT:
      return make_tournament_prediction(pc);
    case CUSTOM:
      return make_custom_prediction(pc);
    default:
      break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void
train_predictor(uint32_t pc, uint8_t outcome)
{
  switch (bpType) {
    case STATIC:
      break;
    case GSHARE:
      train_gshare_predictor(pc, outcome);
      break;
    case TOURNAMENT:
      train_tournament_predictor(pc, outcome);
      break;
    case CUSTOM:
      train_custom_predictor(pc, outcome);
      break;
    default:
      break;
  }
  return;
}
