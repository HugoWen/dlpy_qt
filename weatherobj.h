class weatherobj
{
    private:
        int zthreshold;
        int cthreshold;
        int threshold;
        int thresholdline;
        int trackbar_alpha;

    public:
        void print();
        weatherobj();
        weatherobj(int z,int ct,int t,int l,int a);
        int getz();
        int getct();
        int gett();
        int getl();
        int geta();
};
