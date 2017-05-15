#pragma once

class Alpha {
public:
   void Talk();
};

class Beta {
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
