class Branch {
  public:
    String name;
    Branch * branches[8];
    
  Branch(String name) {
    this->name = name;  
  }
};
