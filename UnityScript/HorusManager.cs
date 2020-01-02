using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;
using System;
using System.IO;
using B83.Image.BMP;
public class HorusManager : MonoBehaviour {

	[DllImport("horus", EntryPoint = "calculateVisibility")]
	public static extern IntPtr getVisibilityResults();

	[DllImport("horus", EntryPoint = "Terminate")]
	public static extern void DLLTerminate();

	[DllImport("horus", EntryPoint = "processMesh")]
	public static extern void ProcessMesh(int meshID, int[] triangles, float[] vertices, int triangleArraySize, int vertexArraySize);

	[DllImport("horus", EntryPoint = "updateCamera")]
	public static extern void UpdateCamera(float[] vertices);

	[DllImport("horus", EntryPoint = "setBufferSize")]
	public static extern void SetBufferSize(int width, int height);

	[DllImport("horus", EntryPoint = "reset")]
	public static extern void ResetPlugin();

	[DllImport("horus", EntryPoint = "getZbufferPath")]
	public static extern IntPtr GetZbufferFilePath();
	

	List<MeshRenderer> staticRenderers = new List<MeshRenderer>();
	List<MeshFilter> meshFilters = new List<MeshFilter>();
	Camera cameraInst;
	float[] cameraSettings = new float[11];
	Transform transformInst;
	
	// Use this for initialization
	void Start () {
		ResetPlugin();
		cameraInst = gameObject.GetComponent<Camera>();
		SetBufferSize(Screen.width, Screen.height);
		PopulateRenderers();
		SendStaticMeshesToPlugin();
	}
	
	// Update is called once per frame
	void Update () {
		UpdateCameraSettings();
		CalculateVisibility();
	}

	void PopulateRenderers()
	{
		var meshes = (MeshFilter[])UnityEngine.Object.FindObjectsOfType(typeof(MeshFilter));
		foreach (var mesh in meshes) 
		{
			var renderer = mesh.gameObject.GetComponent<MeshRenderer>();
			if(renderer)
			{
				meshFilters.Add(mesh);
				staticRenderers.Add(renderer);
			}
		}
	}

	void SendStaticMeshesToPlugin()
	{
		var maxVertexArraySize = GetMaxVertexArraySize();
		float[] vertexArray = new float[maxVertexArraySize];
		for(int i = 0; i < meshFilters.Count; i++)
		{
			var objTransform = meshFilters[i].transform;
			var mesh = meshFilters[i].mesh;
			int vertexIndex = 0;
			for(int t = 0; t <  mesh.vertices.Length; t++)
			{
				var worldPoint = objTransform.TransformPoint(mesh.vertices[t]);
				vertexArray[vertexIndex] = worldPoint.x;
				vertexArray[++vertexIndex] = worldPoint.y;
				vertexArray[++vertexIndex] = worldPoint.z;
				++vertexIndex;
			}
			var vertexArraySize = mesh.vertices.Length * 3;
			var triangleArraySize = mesh.triangles.Length;
			ProcessMesh(i, mesh.triangles, vertexArray, triangleArraySize, vertexArraySize);
		}
	}

	int GetMaxVertexArraySize()
	{
		int largestArraySize = 0;
		foreach (var meshfilter in meshFilters)
		{
			var vertexArraySize = meshfilter.mesh.vertices.Length * 3;
			if(vertexArraySize > largestArraySize) largestArraySize = vertexArraySize;
		}
		return largestArraySize;
	}

	void UpdateCameraSettings()
	{
		cameraSettings[0] = cameraInst.fieldOfView;
		cameraSettings[1] = cameraInst.nearClipPlane;
		cameraSettings[2] = cameraInst.farClipPlane;
		cameraSettings[3] = cameraInst.aspect;
		
		//var worldPos = transform.TransformPoint(transform.position);
		var worldPos = transform.position;
		cameraSettings[4] = worldPos.x;
		cameraSettings[5] = worldPos.y;
		cameraSettings[6] = worldPos.z;

		cameraSettings[7] = transform.rotation.x;
		cameraSettings[8] = transform.rotation.y;
		cameraSettings[9] = transform.rotation.z;
		cameraSettings[10] = transform.rotation.w;

		UpdateCamera(cameraSettings);
	}

	void CalculateVisibility()
	{
		var visibileIndices = MarshallVisibilityResults();
        if (visibileIndices == null)
        {
            foreach (MeshRenderer v in staticRenderers)
            {
                v.enabled = false;
            }
			return;
        }
		printVisible(visibileIndices);
        for (int t = 0; t < staticRenderers.Count; t++)
        {
			if(isVisible(visibileIndices, t)) staticRenderers[t].enabled = true;
			else staticRenderers[t].enabled = false;
        }
	}

	void printVisible(int[] visibleMeshIDs)
	{
		string visible = "";
		for(int i= 1; i < visibleMeshIDs.Length; i++)
		{
			visible += visibleMeshIDs[i].ToString() + ", ";
		}
	}

	bool isVisible(int[] visibleMeshIDs, int meshID)
	{
		for(int i= 1; i < visibleMeshIDs.Length; i++)
		{
			if(visibleMeshIDs[i] == meshID) return true;
		}
		return false;
	}
/*
	void ReadBufferToTexture()
	{
		string path = Marshal.PtrToStringAnsi(GetZbufferFilePath());
		Debug.Log(path);
		Texture2D tex = new Texture2D(Screen.width / 4, Screen.height / 4);
		byte[] fileData = File.ReadAllBytes(path);
		BMPLoader bmpLoader = new BMPLoader();
        BMPImage bmpImg = bmpLoader.LoadBMP(fileData);
        tex = bmpImg.ToTexture2D();
		quadObject.GetComponent<Renderer>().material.mainTexture = tex;
	}
 */
	int[] MarshallVisibilityResults()
	{
		var resultCount = new int[1];		
		
		IntPtr MeshIndicePointer = getVisibilityResults();
		Marshal.Copy(MeshIndicePointer, resultCount, 0, 1);
		if (resultCount[0] == 0) return null;
		int arraySize = resultCount[0] + 1;
		var results = new int[arraySize] ;
		Marshal.Copy(MeshIndicePointer, results, 0, arraySize);
		return results;
	}

}
