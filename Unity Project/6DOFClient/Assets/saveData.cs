using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class saveData : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        if (PlayerPrefs.HasKey("host"))
        {
            gameObject.GetComponent<InputField>().text = PlayerPrefs.GetString("host");
            Debug.Log(PlayerPrefs.GetString("host"));
        }
        else
        {
            Debug.Log("No");
        }
    }

    // Update is called once per frame
    void Update()
    {
    }

}
