hw9: 
	gcc tlpi_hdr.h binary_sems.h semun.h svshm_xfr.h reader.c binary_sems.c -o reader -lrt
	gcc tlpi_hdr.h binary_sems.h semun.h svshm_xfr.h writer.c binary_sems.c -o writer -lrt

clean:
	rm reader
	rm writer