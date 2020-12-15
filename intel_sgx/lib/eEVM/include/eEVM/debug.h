//
// Created by lntee on 10/2/19.
//

#ifndef LNTEE_CONFIG_H
#define LNTEE_CONFIG_H

//#define  LNTEE_DEBUG
//#define  LNTEE_INFO
//#define  LNTEE_LOG

#ifdef LNTEE_INFO
#define INFO() \
    fprintf(stderr, ">>>LOG: %s: %s: %d \n", __FILE__,__func__, __LINE__);
#else
#define INFO(msg) do{}while(0);
#endif

#ifdef LNTEE_LOG
#define LOG(msg) \
    do{\
    fprintf(stderr, "===INFO: %s: %s: %d => ", __FILE__,__func__, __LINE__); \
    fprintf(stderr, "%s\n", msg);\
    }while(0);
#else
#define LOG(msg) do{}while(0);
#endif

#ifdef LNTEE_DEBUG
#define DEBUG(msg) \
    do{\
    fprintf(stderr, "###DEBUG: %s: %s: %d => ", __FILE__,__func__, __LINE__); \
    fprintf(stderr, "%s\n", msg);\
    }while(0);
#else
#define DEBUG(msg) do{}while(0);
#endif

#endif //LNTEE_CONFIG_H
