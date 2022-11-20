using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Reposition : MonoBehaviour
{

    public bool reposition = false;
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        if (reposition)
        {
            gameObject.transform.position = -Camera.main.transform.localPosition;
        }
    }

  

}
