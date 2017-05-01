#ifndef COPILOT_DEBUG
#ifdef DEBUG
#define LOG(msg) cout<<msg<<endl;
#else
#define LOG(msg)
#endif
#endif