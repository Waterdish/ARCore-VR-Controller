using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RotationSet : MonoBehaviour
{
    bool settingRotation;
    Vector3 rotStart;
    float difference;
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        if (settingRotation)
        {
            difference = Camera.main.transform.localEulerAngles.y - rotStart.y;
            if (Mathf.Abs(difference) > 180f)
            {
                difference = difference-360;
            }
            gameObject.transform.Rotate(new Vector3(0f, 0.01f*(difference), 0f));
        }
    }

    public void setRotation()
    {
        if (settingRotation)
        {
            settingRotation = false;
        }
        else
        {
            settingRotation = true;
            rotStart = Camera.main.transform.localEulerAngles;
        }
        
    }
}
