#pragma once

class AlphaBetaParent {
public:
   virtual void Talk() = 0;
};

class Alpha: public AlphaBetaParent {
public:
   void Talk();
   Alpha();
};

class Beta: public AlphaBetaParent {
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

class NamedObject {
public:
   NamedObject(const char* name);
   void Talk();

private:
   const char* name; 
};
