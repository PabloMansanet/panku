#pragma once

class MultiParent {
public:
   virtual void Talk() = 0;
};

class Alpha: public MultiParent {
public:
   void Talk();
   Alpha();
private:
   Alpha(const Alpha&);
   const Alpha& operator=(const Alpha&);
};

class Beta: public MultiParent {
public:
   Beta(Alpha& alpha);
   void Talk();
private:
   int meaningOfLife;
};

class Gamma {
public:
   Gamma(Alpha& alpha, Beta& beta);
   void Talk();

private:
   int meaningOfLife;
};

class NamedObject: public MultiParent {
public:
   NamedObject(const char* name);
   void Talk();

private:
   const char* name; 
};
