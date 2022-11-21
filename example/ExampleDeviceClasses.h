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

class Delta: public MultiParent {
public:
   void Talk();
   Delta();
private:
   Delta(const Delta&);
   const Delta& operator=(const Delta&);
};

class Epsilon: public MultiParent {
public:
   void Talk();
   Epsilon();
private:
   Epsilon(const Epsilon&);
   const Epsilon& operator=(const Epsilon&);
};

class Zeta: public MultiParent {
public:
   void Talk();
   Zeta();
private:
   Zeta(const Zeta&);
   const Zeta& operator=(const Zeta&);
};

class Eta: public MultiParent {
public:
   void Talk();
   Eta(Zeta&);
private:
   Eta(const Eta&);
   const Eta& operator=(const Eta&);
};


