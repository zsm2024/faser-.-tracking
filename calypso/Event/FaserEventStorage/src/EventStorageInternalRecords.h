
#ifndef FASEREVENTSTORAGE_INTERNAL_RECORDS_H
#define FASEREVENTSTORAGE_INTERNAL_RECORDS_H


namespace FaserEventStorage {
  
  struct internal_run_parameters_record {
    uint32_t marker;              
    uint32_t record_size;         
    uint32_t run_number;          
    uint32_t max_events;          
    uint32_t rec_enable;          
    uint32_t trigger_type;        
    uint32_t detector_mask_1of4;
    uint32_t detector_mask_2of4;
    uint32_t detector_mask_3of4;
    uint32_t detector_mask_4of4;
    uint32_t beam_type;           
    uint32_t beam_energy;         
  };

  struct v5_internal_run_parameters_record {
    uint32_t marker;              
    uint32_t record_size;         
    uint32_t run_number;          
    uint32_t max_events;          
    uint32_t rec_enable;          
    uint32_t trigger_type;        
    uint32_t detector_mask_1of2;
    uint32_t detector_mask_2of2;
    uint32_t beam_type;           
    uint32_t beam_energy;         
  };

  struct v2_internal_run_parameters_record {
    uint32_t marker;              
    uint32_t record_size;         
    uint32_t run_number;          
    uint32_t max_events;          
    uint32_t rec_enable;          
    uint32_t trigger_type;        
    uint32_t detector_mask;
    uint32_t beam_type;           
    uint32_t beam_energy;         
  };

  
  const internal_run_parameters_record run_parameters_pattern = {
    RUN_PARAMETERS_MARKER,
    sizeof(internal_run_parameters_record)/sizeof(uint32_t),
    0,    
    0,    
    0,    
    0,
    0,
    0,
    0, 
    0,
    0,     
    0
  };   

  const v2_internal_run_parameters_record v2_run_parameters_pattern = {
    RUN_PARAMETERS_MARKER,
    sizeof(v2_internal_run_parameters_record)/sizeof(uint32_t),
    0,    
    0,    
    0,  
    0, 
    0,
    0,     
    0
  };   

  const v5_internal_run_parameters_record v5_run_parameters_pattern = {
    RUN_PARAMETERS_MARKER,
    sizeof(v5_internal_run_parameters_record)/sizeof(uint32_t),
    0,    
    0,    
    0,  
    0, 
    0,
    0,
    0,     
    0
  };   

}

#endif
