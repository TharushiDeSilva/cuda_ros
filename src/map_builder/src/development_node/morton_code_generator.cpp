// This is the real Hello World for CUDA!
//It takes the string 'Hello', prints it, then passes it to cuda with an array of offsets
// Then the offsets are added in parallel to produce the string world! 
#include <iostream>
#include <bits/stdc++.h> 
#include <stdint.h>
#include <fstream>
#include <time.h>
#include <limits.h> 
#include <bitset>

using namespace std; 
const float resolution = 0.05f; 
float res_list_5cm[] = {3276.8, 1638.4, 819.2, 409.6, 204.8, 102.4, 51.2, 25.6, 12.8, 6.4, 3.2, 1.6, 0.8, 0.4, 0.2, 0.1, 0.05, 0.025}; // in meters
int axis_length = 65536; // 2^16; 
int half_axis_length = 32768; 

inline uint64_t generate_morton_code(uint16_t x, uint16_t y, uint16_t z){
    uint64_t x_original = uint64_t(x);
    uint64_t y_original = uint64_t(y);
    uint64_t z_original = uint64_t(z);
    
    // x_oroginal = 0000000000000000 0000000000000000 0000000000000000 1111111111111111
    uint64_t x_temp = (x_original<<32) & (0b0000000000000000100000000000000000000000000000000000000000000000); 
    // x_temp = 0000000000000000 1000000000000000 0000000000000000 0000000000000000
    x_temp = ((x_original<<30) & (0b0000000000000000000100000000000000000000000000000000000000000000)) | x_temp; 
    // x_temp = 0000000000000000 1001000000000000 0000000000000000 0000000000000000
    x_temp = ((x_original<<28) & (0b0000000000000000000000100000000000000000000000000000000000000000)) | x_temp;
    // x_temp = 0000000000000000 1001001000000000 0000000000000000 0000000000000000
    x_temp = ((x_original<<26) & (0b0000000000000000000000000100000000000000000000000000000000000000)) | x_temp;
    // x_temp = 0000000000000000 1001001001000000 0000000000000000 0000000000000000
    x_temp = ((x_original<<24) & (0b0000000000000000000000000000100000000000000000000000000000000000)) | x_temp;
    // x_temp = 0000000000000000 1001001001001000 0000000000000000 0000000000000000
    x_temp = ((x_original<<22) & (0b0000000000000000000000000000000100000000000000000000000000000000)) | x_temp;
    // x_temp = 0000000000000000 1001001001001001 0000000000000000 0000000000000000
    x_temp = ((x_original<<20) & (0b0000000000000000000000000000000000100000000000000000000000000000)) | x_temp;
    // x_temp = 0000000000000000 1001001001001001 0010000000000000 0000000000000000
    x_temp = ((x_original<<18) & (0b0000000000000000000000000000000000000100000000000000000000000000)) | x_temp;
    // x_temp = 0000000000000000 1001001001001001 0010010000000000 0000000000000000
    x_temp = ((x_original<<16) & (0b0000000000000000000000000000000000000000100000000000000000000000)) | x_temp;
    // x_temp = 0000000000000000 1001001001001001 0010010010000000 0000000000000000
    x_temp = ((x_original<<14) & (0b0000000000000000000000000000000000000000000100000000000000000000)) | x_temp;
    // x_temp = 0000000000000000 1001001001001001 0010010010010000 0000000000000000
    x_temp = ((x_original<<12) & (0b0000000000000000000000000000000000000000000000100000000000000000)) | x_temp;
    // x_temp = 0000000000000000 1001001001001001 0010010010010010 0000000000000000
    x_temp = ((x_original<<10) & (0b0000000000000000000000000000000000000000000000000100000000000000)) | x_temp;
    // x_temp = 0000000000000000 1001001001001001 0010010010010010 0100000000000000
    x_temp = ((x_original<<8) &  (0b0000000000000000000000000000000000000000000000000000100000000000)) | x_temp;
    // x_temp = 0000000000000000 1001001001001001 0010010010010010 0100100000000000
    x_temp = ((x_original<<6) &  (0b0000000000000000000000000000000000000000000000000000000100000000)) | x_temp;
    // x_temp = 0000000000000000 1001001001001001 0010010010010010 0100100100000000
    x_temp = ((x_original<<4) &  (0b0000000000000000000000000000000000000000000000000000000000100000)) | x_temp;
    // x_temp = 0000000000000000 1001001001001001 0010010010010010 0100100100100000
    x_temp = ((x_original<<2) &  (0b0000000000000000000000000000000000000000000000000000000000000100)) | x_temp;
    // x_temp = 0000000000000000 1001001001001001 0010010010010010 0100100100100100
    x_temp = (x_temp>>2); 

    uint64_t m_code = x_temp; 

    // y_oroginal = 0000000000000000 0000000000000000 0000000000000000 1111111111111111
    uint64_t y_temp = (y_original<<32) & (0b0000000000000000100000000000000000000000000000000000000000000000); 
    // y_temp = 0000000000000000 1000000000000000 0000000000000000 0000000000000000
    y_temp = ((y_original<<30) & (0b0000000000000000000100000000000000000000000000000000000000000000)) | y_temp; 
    // y_temp = 0000000000000000 1001000000000000 0000000000000000 0000000000000000
    y_temp = ((y_original<<28) & (0b0000000000000000000000100000000000000000000000000000000000000000)) | y_temp;
    // y_temp = 0000000000000000 1001001000000000 0000000000000000 0000000000000000
    y_temp = ((y_original<<26) & (0b0000000000000000000000000100000000000000000000000000000000000000)) | y_temp;
    // y_temp = 0000000000000000 1001001001000000 0000000000000000 0000000000000000
    y_temp = ((y_original<<24) & (0b0000000000000000000000000000100000000000000000000000000000000000)) | y_temp;
    // y_temp = 0000000000000000 1001001001001000 0000000000000000 0000000000000000
    y_temp = ((y_original<<22) & (0b0000000000000000000000000000000100000000000000000000000000000000)) | y_temp;
    // y_temp = 0000000000000000 1001001001001001 0000000000000000 0000000000000000
    y_temp = ((y_original<<20) & (0b0000000000000000000000000000000000100000000000000000000000000000)) | y_temp;
    // y_temp = 0000000000000000 1001001001001001 0010000000000000 0000000000000000
    y_temp = ((y_original<<18) & (0b0000000000000000000000000000000000000100000000000000000000000000)) | y_temp;
    // y_temp = 0000000000000000 1001001001001001 0010010000000000 0000000000000000
    y_temp = ((y_original<<16) & (0b0000000000000000000000000000000000000000100000000000000000000000)) | y_temp;
    // y_temp = 0000000000000000 1001001001001001 0010010010000000 0000000000000000
    y_temp = ((y_original<<14) & (0b0000000000000000000000000000000000000000000100000000000000000000)) | y_temp;
    // y_temp = 0000000000000000 1001001001001001 0010010010010000 0000000000000000
    y_temp = ((y_original<<12) & (0b0000000000000000000000000000000000000000000000100000000000000000)) | y_temp;
    // y_temp = 0000000000000000 1001001001001001 0010010010010010 0000000000000000
    y_temp = ((y_original<<10) & (0b0000000000000000000000000000000000000000000000000100000000000000)) | y_temp;
    // y_temp = 0000000000000000 1001001001001001 0010010010010010 0100000000000000
    y_temp = ((y_original<<8) &  (0b0000000000000000000000000000000000000000000000000000100000000000)) | y_temp;
    // y_temp = 0000000000000000 1001001001001001 0010010010010010 0100100000000000
    y_temp = ((y_original<<6) &  (0b0000000000000000000000000000000000000000000000000000000100000000)) | y_temp;
    // y_temp = 0000000000000000 1001001001001001 0010010010010010 0100100100000000
    y_temp = ((y_original<<4) &  (0b0000000000000000000000000000000000000000000000000000000000100000)) | y_temp;
    // y_temp = 0000000000000000 1001001001001001 0010010010010010 0100100100100000
    y_temp = ((y_original<<2) &  (0b0000000000000000000000000000000000000000000000000000000000000100)) | y_temp;
    // y_temp = 0000000000000000 1001001001001001 0010010010010010 0100100100100100
    y_temp = (y_temp>>1); 

    m_code = (m_code | y_temp); 

    // z_oroginal = 0000000000000000 0000000000000000 0000000000000000 1111111111111111
    uint64_t z_temp = (z_original<<32) & (0b0000000000000000100000000000000000000000000000000000000000000000); 
    // z_temp = 0000000000000000 1000000000000000 0000000000000000 0000000000000000
    z_temp = ((z_original<<30) & (0b0000000000000000000100000000000000000000000000000000000000000000)) | z_temp; 
    // z_temp = 0000000000000000 1001000000000000 0000000000000000 0000000000000000
    z_temp = ((z_original<<28) & (0b0000000000000000000000100000000000000000000000000000000000000000)) | z_temp;
    // z_temp = 0000000000000000 1001001000000000 0000000000000000 0000000000000000
    z_temp = ((z_original<<26) & (0b0000000000000000000000000100000000000000000000000000000000000000)) | z_temp;
    // z_temp = 0000000000000000 1001001001000000 0000000000000000 0000000000000000
    z_temp = ((z_original<<24) & (0b0000000000000000000000000000100000000000000000000000000000000000)) | z_temp;
    // z_temp = 0000000000000000 1001001001001000 0000000000000000 0000000000000000
    z_temp = ((z_original<<22) & (0b0000000000000000000000000000000100000000000000000000000000000000)) | z_temp;
    // z_temp = 0000000000000000 1001001001001001 0000000000000000 0000000000000000
    z_temp = ((z_original<<20) & (0b0000000000000000000000000000000000100000000000000000000000000000)) | z_temp;
    // z_temp = 0000000000000000 1001001001001001 0010000000000000 0000000000000000
    z_temp = ((z_original<<18) & (0b0000000000000000000000000000000000000100000000000000000000000000)) | z_temp;
    // z_temp = 0000000000000000 1001001001001001 0010010000000000 0000000000000000
    z_temp = ((z_original<<16) & (0b0000000000000000000000000000000000000000100000000000000000000000)) | z_temp;
    // z_temp = 0000000000000000 1001001001001001 0010010010000000 0000000000000000
    z_temp = ((z_original<<14) & (0b0000000000000000000000000000000000000000000100000000000000000000)) | z_temp;
    // z_temp = 0000000000000000 1001001001001001 0010010010010000 0000000000000000
    z_temp = ((z_original<<12) & (0b0000000000000000000000000000000000000000000000100000000000000000)) | z_temp;
    // z_temp = 0000000000000000 1001001001001001 0010010010010010 0000000000000000
    z_temp = ((z_original<<10) & (0b0000000000000000000000000000000000000000000000000100000000000000)) | z_temp;
    // z_temp = 0000000000000000 1001001001001001 0010010010010010 0100000000000000
    z_temp = ((z_original<<8) &  (0b0000000000000000000000000000000000000000000000000000100000000000)) | z_temp;
    // z_temp = 0000000000000000 1001001001001001 0010010010010010 0100100000000000
    z_temp = ((z_original<<6) &  (0b0000000000000000000000000000000000000000000000000000000100000000)) | z_temp;
    // z_temp = 0000000000000000 1001001001001001 0010010010010010 0100100100000000
    z_temp = ((z_original<<4) &  (0b0000000000000000000000000000000000000000000000000000000000100000)) | z_temp;
    // z_temp = 0000000000000000 1001001001001001 0010010010010010 0100100100100000
    z_temp = ((z_original<<2) &  (0b0000000000000000000000000000000000000000000000000000000000000100)) | z_temp;
    // z_temp = 0000000000000000 1001001001001001 0010010010010010 0100100100100100
    
    m_code = (m_code | z_temp); 

    return m_code;
}
inline uint64_t shift_input(uint16_t x){
    uint64_t x_original = uint64_t(x);
    x_original = x_original & (0xffff); 
    // x_oroginal = 0000000000000000 0000000000000000 0000000000000000 1111111111111111
    uint64_t x_temp = (x_original<<32) & (0b0000000000000000100000000000000000000000000000000000000000000000); 
    // x_temp = 0000000000000000 1000000000000000 0000000000000000 0000000000000000
    x_temp = ((x_original<<30) & (0b0000000000000000000100000000000000000000000000000000000000000000)) | x_temp; 
    // x_temp = 0000000000000000 1001000000000000 0000000000000000 0000000000000000
    x_temp = ((x_original<<28) & (0b0000000000000000000000100000000000000000000000000000000000000000)) | x_temp;
    // x_temp = 0000000000000000 1001001000000000 0000000000000000 0000000000000000
    x_temp = ((x_original<<26) & (0b0000000000000000000000000100000000000000000000000000000000000000)) | x_temp;
    // x_temp = 0000000000000000 1001001001000000 0000000000000000 0000000000000000
    x_temp = ((x_original<<24) & (0b0000000000000000000000000000100000000000000000000000000000000000)) | x_temp;
    // x_temp = 0000000000000000 1001001001001000 0000000000000000 0000000000000000
    x_temp = ((x_original<<22) & (0b0000000000000000000000000000000100000000000000000000000000000000)) | x_temp;
    // x_temp = 0000000000000000 1001001001001001 0000000000000000 0000000000000000
    x_temp = ((x_original<<20) & (0b0000000000000000000000000000000000100000000000000000000000000000)) | x_temp;
    // x_temp = 0000000000000000 1001001001001001 0010000000000000 0000000000000000
    x_temp = ((x_original<<18) & (0b0000000000000000000000000000000000000100000000000000000000000000)) | x_temp;
    // x_temp = 0000000000000000 1001001001001001 0010010000000000 0000000000000000
    x_temp = ((x_original<<16) & (0b0000000000000000000000000000000000000000100000000000000000000000)) | x_temp;
    // x_temp = 0000000000000000 1001001001001001 0010010010000000 0000000000000000
    x_temp = ((x_original<<14) & (0b0000000000000000000000000000000000000000000100000000000000000000)) | x_temp;
    // x_temp = 0000000000000000 1001001001001001 0010010010010000 0000000000000000
    x_temp = ((x_original<<12) & (0b0000000000000000000000000000000000000000000000100000000000000000)) | x_temp;
    // x_temp = 0000000000000000 1001001001001001 0010010010010010 0000000000000000
    x_temp = ((x_original<<10) & (0b0000000000000000000000000000000000000000000000000100000000000000)) | x_temp;
    // x_temp = 0000000000000000 1001001001001001 0010010010010010 0100000000000000
    x_temp = ((x_original<<8) &  (0b0000000000000000000000000000000000000000000000000000100000000000)) | x_temp;
    // x_temp = 0000000000000000 1001001001001001 0010010010010010 0100100000000000
    x_temp = ((x_original<<6) &  (0b0000000000000000000000000000000000000000000000000000000100000000)) | x_temp;
    // x_temp = 0000000000000000 1001001001001001 0010010010010010 0100100100000000
    x_temp = ((x_original<<4) &  (0b0000000000000000000000000000000000000000000000000000000000100000)) | x_temp;
    // x_temp = 0000000000000000 1001001001001001 0010010010010010 0100100100100000
    x_temp = ((x_original<<2) &  (0b0000000000000000000000000000000000000000000000000000000000000100)) | x_temp;
    // x_temp = 0000000000000000 1001001001001001 0010010010010010 0100100100100100
    //x_temp = (x_temp>>2); 
    // x_temp = 0000000000000000 1001001001001001 0010010010010010 0100100100100100
    uint64_t m_code = x_temp;
    return m_code;
}

inline uint32_t create_node_value(uint8_t r, uint8_t g, uint8_t b, int8_t occ){
    uint32_t r_temp = uint32_t(r); 
    uint32_t g_temp = uint32_t(g); 
    uint32_t b_temp = uint32_t(b); 
    uint32_t occ_temp = uint32_t(occ);
    uint32_t result = (r_temp<<24) | (g_temp<<16) | (b_temp<<8) | occ_temp; 
    return result; 
}

int main(){ 

    int array_size = 640*480; 
	uint16_t *x_rounded, *y_rounded, *z_rounded; 							// the intermediate results after rounding off the x, y, z, original values to the resolution 
	

	int size_float_arr = array_size * sizeof(float);
    int size_int_arr = array_size * sizeof(uint16_t);
    x_rounded = (uint16_t *)malloc( size_int_arr );
   	y_rounded = (uint16_t *)malloc( size_int_arr );
	z_rounded = (uint16_t *)malloc( size_int_arr );

	ifstream ifile;
    ifile.open ("free_endpoints.txt", ios::in);
    float x_map, y_map, z_map;  
    int count = 0; 
    while(ifile >>x_map>>y_map>>z_map){
        x_rounded[count] = uint16_t(half_axis_length + ceilf(x_map / resolution) -1 );
        y_rounded[count] = uint16_t(half_axis_length + ceilf(y_map / resolution) -1 );
        z_rounded[count] = uint16_t(half_axis_length + ceilf(z_map / resolution) -1 );

        // uint16_t x_temp = uint16_t(half_axis_length + ceilf(x_map / resolution) -1 );
        // uint16_t y_temp = uint16_t(half_axis_length + ceilf(y_map / resolution) -1 );
        // uint16_t z_temp = uint16_t(half_axis_length + ceilf(z_map / resolution) -1 );

        count+=1; 
    }
    ifile.close(); 
	
    // for(int i=0; i<array_size; i++){
    //     std::cout<<"["<<i<<"]"<<x_rounded[i]<<","<<y_rounded[i]<<","<<z_rounded[i]<<std::endl; 
    // }
    
    double start1, end1; 
	start1 = clock();
    uint64_t v = generate_morton_code(32833,32802,32794);
    //uint32_t v = create_node_value(uint8_t(255), uint8_t(0), uint8_t(255), uint8_t(15));
    end1 = clock();
    std::cout <<std::bitset<64>(v)<<std::endl; 
	//std::cout <<std::bitset<32>(v)<<std::endl; 
    double time1 = (double)(end1 - start1); 
	std::cout<<"time to process one dimension: "<<time1<<std::endl; 
	free(x_rounded); 
	free(y_rounded); 
	free(z_rounded);
	
	return EXIT_SUCCESS; 	
}