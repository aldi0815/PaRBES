#include "solver1.h"


#define max_number_of_rhs_variables 100000000 // fix the max_number_of_rhs_variables!

using namespace std;


typedef struct 
{
  uint32_t first;
  uint32_t size;
  set<uint32_t>* succs;
  uint32_t succs_size;
  set<uint32_t>* preds;
}block_t;  

vector< block_t > prepared_blocks;
vector< block_t > new_prepared_blocks;

block_t* all_blocks; 

uint32_t number_of_blocks = 0;
uint32_t solved_blocks = 0;



lhsVariable lefts;
uint32_t rights;

solver1::solver1(void)
{
}


solver1::~solver1(void)
{
}


void* solver1::createDataStructure(bes)
{
	void* dataStruct = NULL;

	printf("creating data structure\n");

	return dataStruct;
}

bool solver1::solve(void*)
{
	bool res = false;
	printf("solving\n");

	return res;
}

std::pair <lhsVariable*, uint32_t*> solver1::get_array_representation_of_bes (bes* input_bes, uint32_t* left_size, uint32_t* right_size)
{
  
  lhsVariable* lefts;
  uint32_t* rights;
  uint32_t* offsets;
  uint32_t offset = 0, index = 0;
  
  number_of_blocks = input_bes->blockCount;
  
  all_blocks = (block_t*) malloc( number_of_blocks * sizeof (block_t) );
  offsets = (uint32_t*) malloc( number_of_blocks * sizeof (uint32_t) );
   
  for(uint32_t i = 0; i< number_of_blocks; i++)
  {
    offsets[i] = offset;
    
    //create all_blocks
    block_t block;
    block.first =  offset;
    block.size = input_bes->blocks[i].eqnCount;
    block.succs = new set<uint32_t>;
    block.succs_size = 0;
    block.preds = new set<uint32_t>;
    all_blocks[i] = block;
    
    
    offset = offset + input_bes->blocks[i].eqnCount;
    
  }

  lefts = (lhsVariable*) malloc ( (offset + 1) * sizeof (lhsVariable) );
  rights = (uint32_t*) malloc (max_number_of_rhs_variables * sizeof (uint32_t) ); // fix the max_number_of_rhs_variables

  offset = 0;
  for(uint32_t i = 0; i< number_of_blocks; i++)
  {
    
    for(uint32_t j = 0; j <  input_bes->blocks[i].eqnCount; j++)
    {
      lefts[offset].setIndex(index);

      if (input_bes->blocks[i].eqns[j].varCount == 1)
      {
        lefts[offset].setSolved();
	lefts[offset].setCon();
	
 	if (input_bes->blocks[i].eqns[j].rhs[0].type == 1)
	{  
	  lefts[offset].setTrue();
	}
	else if (input_bes->blocks[i].eqns[j].rhs[0].type == 0)
	     {
	        lefts[offset].setFalse();
	     }
	     else //TODO prepare for optimization
	     { 
	       //cout << "warning just one variable on rhs" << endl;
	       
	       lefts[offset].setUnsolved();
	       
	       if (input_bes->blocks[i].sign == 1) 
	       { 
		  lefts[offset].setTrue();
		  lefts[offset].setCon();
	       }	  
	       else
	       {	 
		  lefts[offset].setFalse();
		  lefts[offset].setDis();
	       }  
	       
	       if (input_bes->blocks[i].eqns[j].rhs[0].type == local)
	       { 
	         rights[index] = input_bes->blocks[i].eqns[j].rhs[0].localRef + offsets[i];
	       }
	       else{
		 uint32_t globalRef = input_bes->blocks[i].eqns[j].rhs[0].globalRef;
		 
                 rights[index] = input_bes->blocks[i].eqns[j].rhs[0].localRef + offsets[globalRef];
		 
		 if ( i != globalRef )
		 {
		   if (all_blocks[i].succs->insert(globalRef).second)
		   {  
		     all_blocks[i].succs_size++;
		     all_blocks[globalRef].preds->insert(i);
		   }  		     
		 }  
		   
	       }

               index++;
	       if (index >=  max_number_of_rhs_variables)
	       {
	         cout<< "Too much rhs variables" << endl;
	         exit(EXIT_FAILURE);
	       }    
	     }  
      }
      else{
	lefts[offset].setUnsolved();
	
	 if (input_bes->blocks[i].sign == 1) 
	   lefts[offset].setTrue();
	 else
	   lefts[offset].setFalse();
	
 	if (input_bes->blocks[i].eqns[j].rhs[1].type == conjunct) 
	  lefts[offset].setCon();
	else 
          lefts[offset].setDis();	

        for(uint32_t k = 0; k < input_bes->blocks[i].eqns[j].varCount; k=k+2) //TODO fix x1 = x5 and x5 and x5
	{
	  if (input_bes->blocks[i].eqns[j].rhs[k].type == local)
	  { 
	    rights[index] = input_bes->blocks[i].eqns[j].rhs[k].localRef + offsets[i];
	  }
	  else{
	     uint32_t globalRef = input_bes->blocks[i].eqns[j].rhs[k].globalRef;
		 
             rights[index] = input_bes->blocks[i].eqns[j].rhs[k].localRef + offsets[globalRef];
	     
             if ( i != globalRef )
             {
		if (all_blocks[i].succs->insert(globalRef).second)
		{  
		  all_blocks[i].succs_size++;
		  all_blocks[globalRef].preds->insert(i);
		}  		     
	     }  
	  }
          index++;
	  if (index >=  max_number_of_rhs_variables)
	  {
	    cout<< "Too much rhs variables" << endl;
	    exit(EXIT_FAILURE);
	  }  
        }
      }
      
      offset++;
    }
    if( all_blocks[i].succs_size == 0) prepared_blocks.push_back(all_blocks[i]);
  }

  lefts[offset].setIndex(index-1); // set then end of rights

  *left_size = offset; 
  *right_size = index;
  
  free(offsets);
 
  for (uint32_t i = 0; i < number_of_blocks; i++)
  {
    cout << i << " first = " << all_blocks[i].first << " size = " << all_blocks[i].size << " succ_size = " << all_blocks[i].succs_size << endl;
    cout << " succs = ";
     for (set<uint32_t>::iterator j = all_blocks[i].succs->begin(); j != all_blocks[i].succs->end(); j++)
       cout << *j << " ";  
     cout<< endl;
     cout << " preds = ";
      for (set<uint32_t>::iterator j =all_blocks[i].preds->begin(); j != all_blocks[i].preds->end(); j++)
       cout << *j << " ";  
     cout<< endl;
  }  
    
  
  return make_pair(lefts, rights);
}


void solver1::print_array_representation_of_bes (lhsVariable* lefts, uint32_t* rights, uint32_t left_size)
{					 
  for (uint32_t i = 0; i < left_size; i++)
  {
	  printf("%d\n", i);

    cout << i << " value = " << lefts[i].getValue() << " type " << lefts[i].getType() << " value " << lefts[i].isSolved() << " index = " <<  lefts[i].getIndex() << endl;
    if (lefts[i].isSolved())
      cout << " sloved =  " << lefts[i].getValue();
    else 
    {  
      cout << " usloved =  ";
      for (uint32_t j = lefts[i].getIndex(); j < lefts[i+1].getIndex(); j++)
        cout << rights[j] << " ";
    }
    cout << endl;
  }
}
