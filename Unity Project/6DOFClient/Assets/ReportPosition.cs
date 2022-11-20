using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Net.Sockets;
using UnityEngine;
using UnityEngine.UI;
using System;

public class ReportPosition : MonoBehaviour
{
    private Camera mainCamera;

    private bool reposition;

    public Vector3 camPos;
    public Vector3 offset = new Vector3(0f, 1.2f, 0f);
    public Vector3 repositionStart;
    public Quaternion camRot;
    public float euleryOffset;
    public float radiansoffset;
    public float Trig;
    public float Grip;
    public float AButton;
    public float BButton;
    public float Sys;
    public Joystick joystick;
    public Dropdown dropdown;
    public string controller="R";
    public GameObject onscreenbuttons;
    public GameObject sessionOrigin;
    public GameObject connectbuttons;
    public GameObject UI_AButton;
    public GameObject UI_BButton;


    //keeping these here just for reference.
    float swapxb = 1f;
    float swapyb = 1f;
    float swapzb = -1f;

    float leftright = 1f;
    // Start is called before the first frame update
    void Start()
    {
        offset = new Vector3(0f, 1.2f, 0f);
        if (PlayerPrefs.HasKey("Hand"))
        {
            controller = PlayerPrefs.GetString("Hand");
            dropdown.value = (controller == "R" ? 0 : 1);
        }
        else
        {
            controller = "R";
        }
        mainCamera = Camera.main;
        //networkEnable();
        networkEnable();
    }


    // Update is called once per frame
    void Update()
    {
        camPos = mainCamera.transform.position;
        //rotate position coordinates into new system
        float origx = camPos.x;
        float origz = camPos.z;
        camPos.x = (Mathf.Cos(-radiansoffset) * origx) + (Mathf.Sin(-radiansoffset) * origz);
        camPos.z = (-Mathf.Sin(-radiansoffset) * origx) + (Mathf.Cos(-radiansoffset) * origz);
        camPos += offset;
        camRot = mainCamera.transform.localRotation;
        camRot.eulerAngles = new Vector3(camRot.eulerAngles.x, camRot.eulerAngles.y - euleryOffset, camRot.eulerAngles.z);

        if (reposition)
        {
            //  offset += 0.01f * (mainCamera.transform.position - repositionStart);
            //sets rotation to be pointing straight ahead for calibration purposes.
            camRot = Quaternion.Euler(0f, 0f, 0f);
            camRot *= Quaternion.Euler(Vector3.right * +90);
            camPos = offset;
        }
        DisplayCamera();
    }

    /*public void setReposition()
    {
        if (!reposition)
        {
            reposition = true;
            repositionStart = mainCamera.transform.position;
        }
        else
        {
            reposition = false;
        }
    }*/

    public void setReposition()
    {
        if (!reposition)
        {
            reposition = true;
            if (controller == "R")
            {
                offset = new Vector3(0.08f, 1f, 0.2f);
            }
            else
            {
                offset = new Vector3(-0.08f, 1f, 0.2f);
            }

            euleryOffset = 0f;
        }
        else
        {

            //one-time sets campos to 0,0,0 as relative to offset

            //commenting this out did nothing to help the issue.
            sessionOrigin.transform.rotation = Quaternion.Euler(sessionOrigin.transform.eulerAngles.x, mainCamera.transform.rotation.eulerAngles.y, sessionOrigin.transform.eulerAngles.z);
            
            //fix this by putting the transformed coordinates into here.
            //euleryOffset = (mainCamera.transform.rotation.eulerAngles.y - sessionOrigin.transform.eulerAngles.y);
            euleryOffset = mainCamera.transform.localEulerAngles.y;
            radiansoffset = mainCamera.transform.eulerAngles.y * Mathf.Deg2Rad;// euleryOffset * Mathf.Deg2Rad;

            Vector3 posoffsetter = mainCamera.transform.position;
            float origx = posoffsetter.x;
            float origz = posoffsetter.z;
            posoffsetter.x = (Mathf.Cos(-radiansoffset) * origx) + (Mathf.Sin(-radiansoffset) * origz);
            posoffsetter.z = (-Mathf.Sin(-radiansoffset) * origx) + (Mathf.Cos(-radiansoffset) * origz);
            offset -= posoffsetter;//mainCamera.transform.position;

            reposition = false;
        }
    }


    void DisplayCamera()
    {
        //string tex = "Position:" + camPos.x + "," + camPos.y + "," + camPos.z;
        string tex = "Rotation:" + camRot.w;

        //set camrot to point more up.
        //camRot.eulerAngles = new Vector3(camRot.eulerAngles.x-90f,camRot.eulerAngles.y,camRot.eulerAngles.z); 
         camRot *= Quaternion.Euler(Vector3.right * -135); //90 for straight forward. Better for -180 because always sees ground. maybe -135 tho?
        //I originally commented this out to fix the broken rotation but I just need to actually fix this line. It's not rotating correctly. Vector3.up i think is the issue.
        //camRot *= Quaternion.Euler(Vector3.up * -euleryOffset);
        //writeSocket(tex);
        writeSocket(
            "<"+controller+">"+"</"+controller+">\n"+
            "<Position>\n" +
            "<x>" + camPos.x +
            "</x>\n" +
            "<y>" + camPos.y +
            "</y>\n" +
            "<z>" + camPos.z * -1f +
            "</z>\n" +
            "</Position>\n" +
            "<Rotation>\n" +
            "<x>" + camRot.x +
            "</x>\n" +
            "<y>" + camRot.y +
            "</y>\n" +
            "<z>" + camRot.z * -1f +
            "</z>\n" +
            "<w>" + camRot.w * -1f +
            "</w>\n" +
            "</Rotation>\n" +
            "<Trig>" + Trig +
            "</Trig>\n" +
             "<Grip>" + Grip +
            "</Grip>\n" +
            "<Joy>\n" +
            "<x>" + joystick.Horizontal+
            "</x>\n" +
            "<y>" + joystick.Vertical +//*leftright +
            "</y>\n" +
            "</Joy>\n" +
            "<A>" + AButton +
            "</A>\n" +
             "<B>" + BButton +
            "</B>\n" +
            "<S>" + Sys +
            "</S>\n"
            );
    }

    bool socketReady;
    UdpClient mySocket;
    NetworkStream theStream;
    StreamWriter theWriter;
    StreamReader theReader;
    string Host = "192.168.254.126";
    Int32 Port = 55455;

    void networkEnable()
    {
        try
        {
            mySocket = new UdpClient();
            socketReady = true;
        }
        catch (Exception e)
        {
            Debug.Log("Socket error: " + e);
        }
    }

    private Transform[] transforms;

    public void changeController()
    {
        switch (dropdown.value)
        {
            case 0:
                controller = "R";
                //leftright = 1;
                UI_AButton.GetComponentInChildren<Text>().text = "A";
                UI_BButton.GetComponentInChildren<Text>().text = "B";
                foreach (RectTransform j in onscreenbuttons.GetComponentsInChildren<RectTransform>())
                {
                    if (j.GetComponent<Text>() == null && j.gameObject.name != "Handle" && j.gameObject.name != "OnScreenButtons")
                        j.anchoredPosition = new Vector2(-(j.anchoredPosition.x -66*2), j.anchoredPosition.y);
                }
                break;
            case 1:
                //leftright = -1;
                controller = "L";
                UI_AButton.GetComponentInChildren<Text>().text = "X";
                UI_BButton.GetComponentInChildren<Text>().text = "Y";
                foreach (RectTransform j in onscreenbuttons.GetComponentsInChildren<RectTransform>())
                {
                    if (j.GetComponent<Text>() == null && j.gameObject.name != "Handle" && j.gameObject.name != "OnScreenButtons")
                        j.anchoredPosition = new Vector2(-j.anchoredPosition.x + (66*2), j.anchoredPosition.y);
                }
                break;
        }
        PlayerPrefs.SetString("Hand", controller);
    }

    public void createHost()
    {
        Text inputtext = GameObject.FindGameObjectWithTag("HostText").GetComponent<Text>();
        if(inputtext.text!="")
            Host = inputtext.text.Trim();
        Debug.Log(Host);
        connectbuttons.SetActive(false);
        onscreenbuttons.SetActive(true);
        PlayerPrefs.SetString("host", Host);
    }

    public void writeSocket(string theLine)
    {
        if (!socketReady)
            return;
        String foo = theLine + "\0";
        byte[] bytes = System.Text.Encoding.ASCII.GetBytes(foo);
        mySocket.Send(bytes, bytes.Length,Host,Port);
    }

    void OnApplicationQuit()
    {
        if (!socketReady)
        {
            return;
        }

        mySocket.Close();
        socketReady = false;
    }



    public void sysdown()
    {
        Sys = 1f;
    }
    public void sysup()
    {
        Sys = 0f;
    }
    public void triggerdown()
    {
        Trig = 1f;
    }
    public void triggerup()
    {
        Trig = 0f;
    }
    public void gripdown()
    {
        Grip = 1f;
    }
    public void gripup()
    {
        Grip = 0f;
    }
    public void Adown()
    {
        AButton = 1f;
    }
    public void Aup()
    {
        AButton = 0f;
    }
    public void Bdown()
    {
        BButton = 1f;
    }
    public void Bup()
    {
        BButton = 0f;
    }

}
