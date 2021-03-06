/*
 * cps_api_key_utils.cpp
 *
 *  Created on: Sep 1, 2016
 */

#include "cps_api_key_utils.h"
#include "cps_api_object_key.h"

void cps_api_object_iterate_key_attrs(cps_api_object_t obj, const std::function<void(cps_api_object_t,
								cps_api_attr_id_t,void *,size_t)> &iter) {
	cps_api_key_element_t *p = cps_api_key_elem_start(cps_api_object_key(obj));
    size_t len = cps_api_key_get_len(cps_api_object_key(obj));
    for ( size_t ix = 0; ix < len ; ++ix ) {
        cps_api_object_attr_t attr = cps_api_get_key_data(obj,p[ix]);
        if (attr==nullptr) continue;
        iter(obj,p[ix],cps_api_object_attr_data_bin(attr),cps_api_object_attr_len(attr));
    }
}

size_t cps_api_object_count_key_attrs(cps_api_object_t obj) {
	size_t count = 0;
	cps_api_object_iterate_key_attrs(obj,[&count](cps_api_object_t,
								cps_api_attr_id_t,void *,size_t)->void {
		++count;
	});
	return count;
}

uint64_t cps_api_hash_array(const cps_api_key_element_t *ptr, size_t len) {
    const cps_api_key_element_t *end = ptr + len;

    uint64_t hash = 0;
    std::hash<uint64_t> h;
    while(ptr < end) {
        hash = (hash << 8) | hash>> ((sizeof(hash)*8)-8);
        hash ^= h(*(ptr++));
    }
    return hash;
}

int cps_api_key_array_matches(const cps_api_key_element_t *lhs, size_t src_len,
		const cps_api_key_element_t *rhs, size_t comp_len, bool exact)  {
	//convert to bytes
	src_len *= sizeof(*lhs);
	comp_len *= sizeof(*rhs);

    int match_len = (src_len > comp_len) ? comp_len : src_len;
    int res = memcmp(lhs,rhs,match_len);

    if (exact && src_len!=comp_len) {
        return res!=0 ? res : src_len - comp_len;
    }

    if (comp_len > src_len) {
        return res!=0 ? res : 1;
    }

    return res;
}

