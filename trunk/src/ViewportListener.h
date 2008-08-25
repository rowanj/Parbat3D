#ifndef VIEWPORT_LISTENER_H
#define VIEWPORT_LISTENER_H

class ViewportListener
{
public:
  virtual ~ViewportListener() {};
	virtual void notify_viewport(void);
	virtual void notify_bands(void);
};

#endif
